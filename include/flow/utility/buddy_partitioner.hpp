#pragma once

#include <algorithm>
#include <bit>
#include <concepts>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "../core/assertion.hpp"
#include "../utility/numeric.hpp"

namespace flow {

template<std::integral BlockT, std::integral SizeT = std::size_t>
class buddy_partitioner
{
public:
    using block_type = BlockT;
    using size_type = SizeT;

private:
    static constexpr size_type level_bit_count = 8;
    static constexpr std::size_t max_level_mask = (static_cast<size_type>(1) << level_bit_count) - 1;
    static constexpr size_type nullindex = std::numeric_limits<size_type>::max() >> level_bit_count;

    static_assert(std::numeric_limits<block_type>::digits > level_bit_count, "size_type is too small");

public:
    constexpr buddy_partitioner() noexcept
        : m_block_size{}
        , m_base_offset{}
        , m_blocks{}
        , m_freelists{}
    {}

    constexpr buddy_partitioner(size_type block_count, size_type block_size, size_type base_offset = 0)
        : buddy_partitioner()
    {
        create(block_count, block_size, base_offset);
    }

    constexpr size_type create(size_type block_count, size_type block_size, size_type base_offset = 0)
    {
        size_type pow2_block_count = std::bit_floor(block_count);

        if (pow2_block_count == 0)
        {
            return 0;
        }

        m_block_size = block_size;
        m_base_offset = base_offset;

        size_type levels = fast_log2(pow2_block_count) + 1;
        m_blocks.resize(pow2_block_count, make_block(nullindex, 0));
        m_freelists.resize(levels);

        m_blocks[0] = make_block(0, levels - 1);
        m_freelists.back().push_back(0);

        return pow2_block_count;
    }

    constexpr size_type create(std::span<block_type> blocks,
                               std::span<std::span<size_type>> freelists,
                               size_type block_size,
                               size_type base_offset = 0)
    {
        size_type pow2_block_count = std::bit_floor(blocks.size());

        if (pow2_block_count != blocks.size())
        {
            return 0;
        }

        m_block_size = block_size;
        m_base_offset = base_offset;

        m_blocks.resize(blocks.size());
        m_freelists.resize(freelists.size());

        std::copy(blocks.begin(), blocks.end(), m_blocks.begin());

        for (std::size_t level = 0; level < m_freelists.size(); ++level)
        {
            std::copy(freelists[level].begin(), freelists[level].end(), m_freelists[level].begin());
        }

        return pow2_block_count;
    }

    [[nodiscard]] constexpr std::optional<size_type> alloc(size_type size)
    {
        if (size == 0)
        {
            return std::nullopt;
        }

        size_type request_level = size_to_level(size);

        // find level of the first free block that is
        // at least as big as the requested size
        size_type free_level = request_level;

        while (free_level < m_freelists.size() && m_freelists[free_level].empty())
        {
            ++free_level;
        }

        // the requested block is too big or
        // there are no available free blocks
        if (free_level >= m_freelists.size())
        {
            return std::nullopt;
        }

        // remove the first free block found
        // as it will either be split or used as is
        const size_type block_index = m_freelists[free_level].back();
        m_blocks[block_index] = make_block(nullindex, free_level);
        m_freelists[free_level].pop_back();

        // split the free block until the desired level
        // and add the children to the freelists
        while (free_level > request_level)
        {
            const size_type next_level = --free_level;
            const size_type right_block_index = block_index + (static_cast<size_type>(1) << next_level);

            // we actually only need to add the right child to the freelist,
            // because the left child would be split, thus removed, in the next iteration
            m_blocks[right_block_index] = make_block(m_freelists[next_level].size(), next_level);
            m_freelists[next_level].push_back(right_block_index);
        }

        // update the allocated block
        // no need to remove from the freelist since this can only be
        // either the block that we started from (that is removed before we start spliting)
        // or the left child of the last split block (that we never actually add to the freelist)
        m_blocks[block_index] = make_block(nullindex, free_level);

        return m_base_offset + block_index * m_block_size;
    }

    constexpr void free(size_type offset)
    {
        const size_type relative_offset = offset - m_base_offset;
        size_type block_index = relative_offset / m_block_size;

        FLOW_ASSERT(get_block_index(m_blocks[block_index]) == nullindex, "trying to free unallocated block");

        size_type free_level = get_block_level(m_blocks[block_index]);

        // update the block with the freelist entry, but don't push the block
        // to the freelist yet, since we might merge it with its buddy
        m_blocks[block_index] = make_block(m_freelists[free_level].size(), free_level);

        while (free_level < m_freelists.size() - 1 && !m_freelists[free_level].empty())
        {
            const size_type buddy_block_index = other_buddy(block_index * m_block_size, free_level) / m_block_size;

            // buddy is not in the freelist so we stop the merge process
            if (get_block_index(m_blocks[buddy_block_index]) == nullindex
                || get_block_level(m_blocks[buddy_block_index]) != free_level)
            {
                break;
            }

            // swap the buddy freelist entry with the last one, in order to remove
            // from the back without moving other entries and invalidating their indices
            const size_type buddy_freelist_index = get_block_index(m_blocks[buddy_block_index]);
            const size_type swap_freelist_index = m_freelists[free_level].size() - 1;
            const size_type swap_block_index = m_freelists[free_level][swap_freelist_index];

            using std::swap;
            swap(m_freelists[free_level][buddy_freelist_index],
                 m_freelists[free_level][swap_freelist_index]);
            swap_block_indices(m_blocks[buddy_block_index], m_blocks[swap_block_index]);

            // we only need to remove the buddy from the freelist, because we never
            // push the actual freed block until we finish the merge process
            m_blocks[buddy_block_index] = make_block(nullindex, free_level);
            m_freelists[free_level].pop_back();

            // update the block index with the index of the merged block,
            // which is always the index of the left buddy
            block_index = left_buddy(block_index * m_block_size, free_level) / m_block_size;

            // update the merged block, but don't add it to the freelist
            const size_type next_level = ++free_level;
            m_blocks[block_index] = make_block(m_freelists[next_level].size(), next_level);
        }

        // push the freed block to the freelist
        // no need to update the block entry since this can only be
        // either the block that we started from (that is updated before we start merging)
        // or the last merged block (that we updated in the loop)
        m_freelists[free_level].push_back(block_index);
    }

    [[nodiscard]] constexpr std::span<const block_type> blocks() const noexcept
    {
        return m_blocks;
    }

    [[nodiscard]] constexpr std::span<const std::vector<size_type>> freelists() const noexcept
    {
        return m_freelists;
    }

    [[nodiscard]] constexpr size_type block_size() const noexcept
    {
        return m_block_size;
    }

    [[nodiscard]] constexpr size_type base() const noexcept
    {
        return m_base_offset;
    }

    [[nodiscard]] constexpr size_type block_count() const noexcept
    {
        if (m_freelists.empty())
        {
            return 0;
        }

        return static_cast<size_type>(1) << (m_freelists.size() - 1);
    }

    [[nodiscard]] constexpr size_type capacity() const noexcept
    {
        return m_block_size * block_count();
    }

    [[nodiscard]] constexpr size_type max_allocations(size_type size) const noexcept
    {
        if (size == 0)
        {
            return 0;
        }

        const size_type min_level = size_to_level(size);
        size_type allocations = 0;

        for (size_type level = min_level; level < m_freelists.size(); ++level)
        {
            allocations += m_freelists[level].size() << (level - min_level);
        }

        return allocations;
    }

private:
    [[nodiscard]] constexpr size_type other_buddy(size_type relative_offset, size_type level) const noexcept
    {
        const size_type mask = static_cast<size_type>(1) << level;
        return ((relative_offset / m_block_size) ^ mask) * m_block_size;
    }

    [[nodiscard]] constexpr size_type left_buddy(size_type relative_offset, size_type level) const noexcept
    {
        const size_type mask = (~static_cast<size_type>(0)) << (level + 1);
        return ((relative_offset / m_block_size) & mask) * m_block_size;
    }

    [[nodiscard]] constexpr size_type size_to_level(size_type size) const noexcept
    {
        const auto block_count = size / m_block_size + (size % m_block_size != 0);
        return fast_log2(block_count);
    }

    [[nodiscard]] static constexpr block_type make_block(size_type index, size_type level) noexcept
    {
        return (index << level_bit_count) | (level & max_level_mask);
    }

    [[nodiscard]] static constexpr size_type get_block_index(block_type block) noexcept
    {
        return block >> level_bit_count;
    }

    [[nodiscard]] static constexpr size_type get_block_level(block_type block) noexcept
    {
        return block & max_level_mask;
    }

    static constexpr void swap_block_indices(block_type& lhs, block_type& rhs) noexcept
    {
        const size_type lindex = get_block_index(lhs);
        const size_type rindex = get_block_index(rhs);
        lhs &= (rindex << level_bit_count);
        rhs &= (lindex << level_bit_count);
    }

private:
    size_type m_block_size;
    size_type m_base_offset;
    std::vector<block_type> m_blocks;
    std::vector<std::vector<size_type>> m_freelists;
};

} // namespace flow
