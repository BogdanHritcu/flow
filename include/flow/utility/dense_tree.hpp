#pragma once

#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <vector>

#include "concepts.hpp"
#include "istream.hpp"
#include "ostream.hpp"
#include "traits.hpp"

namespace flow {

namespace detail {

    template<typename TreeT>
    class dfs_iterator
    {
    private:
        using tree_type = TreeT;
        using index_type = typename tree_type::index_type;
        static constexpr auto is_const = std::is_const_v<tree_type>;

        using conditional_const_value_type = std::conditional_t<is_const,
                                                                std::add_const_t<typename tree_type::value_type>,
                                                                typename tree_type::value_type>;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename tree_type::value_type;
        using difference_type = typename tree_type::difference_type;
        using pointer = conditional_const_value_type*;
        using const_pointer = std::add_const_t<value_type>*;
        using reference = conditional_const_value_type&;
        using const_reference = std::add_const_t<value_type>&;

    public:
        constexpr dfs_iterator() noexcept
            : m_tree_ptr{ nullptr }
            , m_index{ tree_type::end_index }
        {}

        constexpr dfs_iterator(tree_type* tree_ptr, index_type index) noexcept
            : m_tree_ptr{ tree_ptr }
            , m_index{ index }
        {}

        constexpr dfs_iterator& operator++() noexcept
        {
            if (m_tree_ptr == nullptr)
            {
                return *this;
            }

            const tree_type& tree = *m_tree_ptr;

            if (m_index == tree_type::before_begin_index)
            {
                m_index = tree.m_root_index;
                return *this;
            }

            if (tree.has_children(m_index))
            {
                m_index = tree.first_child_index_of(m_index);
            }
            else if (tree.has_siblings(m_index))
            {
                m_index = tree.next_sibling_index_of(m_index);
            }
            else
            {
                index_type parent_index = tree.parent_index_of(m_index);
                while (parent_index != tree_type::before_begin_index && !tree.has_siblings(parent_index))
                {
                    parent_index = tree.parent_index_of(parent_index);
                }

                if (tree.has_siblings(parent_index))
                {
                    m_index = tree.next_sibling_index_of(parent_index);
                }
                else
                {
                    m_index = tree_type::end_index;
                }
            }

            return *this;
        }

        constexpr dfs_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        [[nodiscard]] friend constexpr bool operator==(const dfs_iterator&, const dfs_iterator&) noexcept = default;

        [[nodiscard]] constexpr operator bool() const noexcept(noexcept(m_tree_ptr->is_node(m_index)))
        {
            return m_tree_ptr && m_tree_ptr->is_node(m_index);
        }

        template<std::same_as<std::add_const_t<tree_type>> TreeU>
            requires(!is_const)
        [[nodiscard]] constexpr operator dfs_iterator<TreeU>() const noexcept
        {
            return dfs_iterator<TreeU>(m_tree_ptr, m_index);
        }

        [[nodiscard]] constexpr reference operator*() noexcept
            requires(!is_const)
        {
            return m_tree_ptr->node_slots()[m_index].value;
        }

        [[nodiscard]] constexpr const_reference operator*() const noexcept
        {
            return m_tree_ptr->node_slots()[m_index].value;
        }

        [[nodiscard]] constexpr pointer operator->() noexcept
            requires(!is_const)
        {
            return &m_tree_ptr->node_slots()[m_index].value;
        }

        [[nodiscard]] constexpr const_pointer operator->() const noexcept
        {
            return &m_tree_ptr->node_slots()[m_index].value;
        }

    private:
        tree_type* m_tree_ptr;
        index_type m_index;

        friend TreeT;
    };

} // namespace detail

template<typename T, std::unsigned_integral IndexT>
class dense_tree
{
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using index_type = IndexT;

private:
    struct node_indices
    {
        index_type parent;
        index_type first_child;
        index_type next_sibling;
    };

    struct node_value_first
    {
        value_type value;
        node_indices indices;
    };

    struct node_value_last
    {
        node_indices indices;
        value_type value;
    };

    static constexpr index_type before_begin_index = std::numeric_limits<index_type>::max() - 1;
    static constexpr index_type end_index = std::numeric_limits<index_type>::max();

public:
    using node_type = min_size_t<node_value_first, node_value_last>;
    using iterator = detail::dfs_iterator<dense_tree>;
    using const_iterator = detail::dfs_iterator<std::add_const_t<dense_tree>>;

public:
    constexpr dense_tree() noexcept
        : m_root_index{ end_index }
        , m_node_slots{}
        , m_free_slot_indices{}
    {}

    [[nodiscard]] constexpr iterator before_begin() noexcept
    {
        return iterator(this, before_begin_index);
    }

    [[nodiscard]] constexpr const_iterator before_begin() const noexcept
    {
        return const_iterator(this, before_begin_index);
    }

    [[nodiscard]] constexpr const_iterator cbefore_begin() const noexcept
    {
        return const_iterator(this, before_begin_index);
    }

    [[nodiscard]] constexpr iterator begin() noexcept
    {
        return iterator(this, m_root_index);
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator(this, m_root_index);
    }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(this, m_root_index);
    }

    [[nodiscard]] constexpr iterator end() noexcept
    {
        return iterator(this, end_index);
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return const_iterator(this, end_index);
    }

    [[nodiscard]] constexpr const_iterator cend() const noexcept
    {
        return const_iterator(this, end_index);
    }

    [[nodiscard]] constexpr iterator get_iterator(index_type index) noexcept
    {
        return iterator(this, index);
    }

    [[nodiscard]] constexpr const_iterator get_iterator(index_type index) const noexcept
    {
        return const_iterator(this, index);
    }

    [[nodiscard]] constexpr const_iterator get_citerator(index_type index) const noexcept
    {
        return const_iterator(this, index);
    }

    [[nodiscard]] constexpr index_type get_index(const_iterator it) const noexcept
    {
        return it.m_index;
    }

    constexpr iterator insert_after(const_iterator it, const value_type& value)
    {
        const bool is_before_begin_it = it == before_begin();

        if (!(is_before_begin_it || is_node(it)))
        {
            return end();
        }

        // find a free slot
        index_type index{ end_index };
        while (m_free_slot_indices.size() > 0)
        {
            index = m_free_slot_indices.back();
            m_free_slot_indices.pop_back();

            if (!is_node(index))
            {
                break;
            }
        }

        if (is_before_begin_it)
        {
            if (index < m_node_slots.size())
            {
                m_node_slots[index] = make_node(before_begin_index,
                                                m_root_index,
                                                end_index,
                                                value);
            }
            else
            {
                m_node_slots.push_back(make_node(before_begin_index,
                                                 m_root_index,
                                                 end_index,
                                                 value));

                index = static_cast<index_type>(m_node_slots.size() - 1);
            }

            m_root_index = index;
        }
        else
        {
            if (index < m_node_slots.size())
            {
                m_node_slots[index] = make_node(it.m_index,
                                                end_index,
                                                first_child_index_of(it.m_index),
                                                value);
            }
            else
            {
                m_node_slots.push_back(make_node(it.m_index,
                                                 end_index,
                                                 first_child_index_of(it.m_index),
                                                 value));

                index = static_cast<index_type>(m_node_slots.size() - 1);
            }

            node_at(it.m_index).indices.first_child = index;
        }

        return iterator(this, index);
    }

    constexpr iterator erase_after(const_iterator it)
    {
        const bool is_before_begin_it = it == before_begin();

        if (!(is_before_begin_it || is_node(it)))
        {
            return end();
        }

        // simulate a queue by using the already
        // available vector of free indices
        auto queue_start = m_free_slot_indices.size();
        auto queue_end = m_free_slot_indices.size();

        // clang-format off
        auto queue_push = [&](index_type index) { m_free_slot_indices.push_back(index); ++queue_end; };
        auto queue_pop = [&]() { return m_free_slot_indices[queue_start++]; };
        auto queue_size = [&]() { return queue_end - queue_start; };
        // clang-format on

        auto temp_it = it;

        if (is_node(++temp_it))
        {
            queue_push(temp_it.m_index);
        }

        while (queue_size() > 0)
        {
            index_type current_node_index = queue_pop();

            // no need to check if the node is valid, because
            // we do it before pushing them in the queue
            // a node that is invalid corresponds to a visited node

            index_type child_index = first_child_index_of(current_node_index);
            while (is_node(child_index))
            {
                queue_push(child_index);
                child_index = next_sibling_index_of(child_index);
            }

            invalidate_node(node_at(current_node_index));
        }

        if (is_before_begin_it)
        {
            m_root_index = end_index;
            return end();
        }
        else
        {
            auto& node = node_at(first_child_index_of(it.m_index));
            node.indices.first_child = end_index;

            return iterator(this, node.indices.next_sibling);
        }
    }

    [[nodiscard]] constexpr node_type& node_at(const_iterator it) noexcept
    {
        return m_node_slots[it.m_index];
    }

    [[nodiscard]] constexpr const node_type& node_at(const_iterator it) const noexcept
    {
        return m_node_slots[it.m_index];
    }

    [[nodiscard]] constexpr iterator parent_of(const_iterator it) noexcept
    {
        return iterator(*this, parent_index_of(it.m_index));
    }

    [[nodiscard]] constexpr const_iterator parent_of(const_iterator it) const noexcept
    {
        return const_iterator(*this, parent_index_of(it.m_index));
    }

    [[nodiscard]] constexpr iterator first_child_of(const_iterator it) noexcept
    {
        return iterator(*this, first_child_index_of(it.m_index));
    }

    [[nodiscard]] constexpr const_iterator first_child_of(const_iterator it) const noexcept
    {
        return const_iterator(*this, first_child_index_of(it.m_index));
    }

    [[nodiscard]] constexpr iterator next_sibling_of(const_iterator it) noexcept
    {
        return iterator(this, next_sibling_index_of(it.m_index));
    }

    [[nodiscard]] constexpr const_iterator next_sibling_of(const_iterator it) const noexcept
    {
        return const_iterator(this, next_sibling_index_of(it.m_index));
    }

    [[nodiscard]] constexpr bool has_parent(const_iterator it) const noexcept
    {
        return has_parent(it.m_index);
    }

    [[nodiscard]] constexpr bool has_children(const_iterator it) const noexcept
    {
        return has_children(it.m_index);
    }

    [[nodiscard]] constexpr bool has_siblings(const_iterator it) const noexcept
    {
        return has_siblings(it.m_index);
    }

    [[nodiscard]] constexpr bool is_node(const_iterator it) const noexcept
    {
        return is_node(it.m_index);
    }

    [[nodiscard]] constexpr std::vector<node_type>& node_slots() noexcept
    {
        return m_node_slots;
    }

    [[nodiscard]] constexpr const std::vector<node_type>& node_slots() const noexcept
    {
        return m_node_slots;
    }

    [[nodiscard]] constexpr std::vector<index_type>& free_slot_indices() noexcept
    {
        return m_free_slot_indices;
    }

    [[nodiscard]] constexpr const std::vector<index_type>& free_slot_indices() const noexcept
    {
        return m_free_slot_indices;
    }

private:
    [[nodiscard]] constexpr node_type& node_at(index_type index) noexcept
    {
        return m_node_slots[index];
    }

    [[nodiscard]] constexpr const node_type& node_at(index_type index) const noexcept
    {
        return m_node_slots[index];
    }

    [[nodiscard]] constexpr index_type parent_index_of(index_type index) const noexcept
    {
        return m_node_slots[index].indices.parent;
    }

    [[nodiscard]] constexpr index_type first_child_index_of(index_type index) const noexcept
    {
        return m_node_slots[index].indices.first_child;
    }

    [[nodiscard]] constexpr index_type next_sibling_index_of(index_type index) const noexcept
    {
        return m_node_slots[index].indices.next_sibling;
    }

    [[nodiscard]] constexpr bool has_parent(index_type index) const noexcept
    {
        return index < m_node_slots.size() && is_node(m_node_slots[index].indices.parent);
    }

    [[nodiscard]] constexpr bool has_children(index_type index) const noexcept
    {
        return (index < m_node_slots.size() && is_node(m_node_slots[index].indices.first_child))
            || (index == before_begin_index && is_node(m_root_index));
    }

    [[nodiscard]] constexpr bool has_siblings(index_type index) const noexcept
    {
        return index < m_node_slots.size() && is_node(m_node_slots[index].indices.next_sibling);
    }

    [[nodiscard]] constexpr bool is_node(index_type index) const noexcept
    {
        return index < m_node_slots.size()
            && ((index != m_root_index && is_valid_non_root(m_node_slots[index]))
                || (index == m_root_index && is_valid_root(m_node_slots[index])));
    }

    [[nodiscard]] static constexpr bool is_valid_root(const node_type& node) noexcept
    {
        return node.indices.parent == before_begin_index
            && node.indices.first_child != before_begin_index
            && node.indices.next_sibling == end_index;
    }

    [[nodiscard]] static constexpr bool is_valid_non_root(const node_type& node) noexcept
    {
        return node.indices.parent != before_begin_index
            && node.indices.parent != end_index
            && node.indices.first_child != before_begin_index
            && node.indices.next_sibling != before_begin_index;
    }

    static constexpr void invalidate_node(node_type& node) noexcept
    {
        node.indices.parent = end_index;
        node.indices.first_child = before_begin_index;
        node.indices.next_sibling = before_begin_index;
    }

    // clang-format off

    template<typename... Args>
    [[nodiscard]] static constexpr node_type make_node(index_type parent_index,
                                                       index_type first_child_index,
                                                       index_type next_sibling_index,
                                                       Args&&... args)
        noexcept(noexcept(std::is_nothrow_constructible_v<value_type, Args...>))
    {
        if constexpr (std::same_as<node_type, node_value_first>)
        {
            return node_type{
                .value = { std::forward<Args>(args)... },
                .indices = {
                    .parent = parent_index,
                    .first_child = first_child_index,
                    .next_sibling = next_sibling_index }
            };
        }
        else
        {
            return node_type{
                .indices = {
                    .parent = parent_index,
                    .first_child = first_child_index,
                    .next_sibling = next_sibling_index },
                .value = { std::forward<Args>(args)... }
            };
        }
    }

    // clang-format on

    // clang-format off

    template<typename... Args>
    [[nodiscard]] static constexpr node_type make_node(index_type parent_index, Args&&... args)
        noexcept(noexcept(make_node(parent_index,
                                    end_index,
                                    end_index,
                                    std::forward<Args>(args)...)))
    {
        return make_node(parent_index,
                         end_index,
                         end_index,
                         std::forward<Args>(args)...);
    }

    // clang-format on
private:
    index_type m_root_index;
    std::vector<node_type> m_node_slots;
    std::vector<index_type> m_free_slot_indices;

    friend class detail::dfs_iterator<dense_tree>;
    friend class detail::dfs_iterator<std::add_const_t<dense_tree>>;
};

template<typename DataT, std::unsigned_integral IndexT>
struct serialization_traits<dense_tree<DataT, IndexT>>
{
    using size_type = IndexT;
};

template<typename DataT, std::unsigned_integral IndexT>
struct serializer<dense_tree<DataT, IndexT>>
{
    using node_type = typename dense_tree<DataT, IndexT>::node_type;
    using traits = serialization_traits<dense_tree<DataT, IndexT>>;

    void operator()(ostream_view& out, const dense_tree<DataT, IndexT>& t) const
    {
        if constexpr (concepts::trivially_copyable<node_type>)
        {
            using nodes_container_type = decltype(t.node_slots());
            out.serialize<nodes_container_type, traits>(t.node_slots());
        }
        else
        {
            const auto size = static_cast<IndexT>(t.node_slots().size());
            out.serialize(size);

            for (const auto& node : t.node_slots() | std::views::take(size))
            {
                out.serialize(node,
                              [](ostream_view& out, const node_type& n)
                              {
                                  out.serialize(n.value)
                                      .serialize(n.indices);
                              });
            }
        }

        using indices_container_type = decltype(t.free_slot_indices());
        out.serialize<indices_container_type, traits>(t.free_slot_indices());
    }
};

template<typename DataT, std::unsigned_integral IndexT>
struct deserializer<dense_tree<DataT, IndexT>>
{
    using node_type = typename dense_tree<DataT, IndexT>::node_type;
    using traits = serialization_traits<dense_tree<DataT, IndexT>>;

    void operator()(istream_view& in, dense_tree<DataT, IndexT>& t) const
    {
        if constexpr (concepts::trivially_copyable<node_type>)
        {
            using nodes_container_type = decltype(t.node_slots());
            in.deserialize<nodes_container_type, traits>(t.node_slots());
        }
        else
        {
            IndexT size{};
            in.deserialize(size);

            t.node_slots().resize(size);

            for (const auto& node : t.node_slots())
            {
                in.deserialize(node,
                               [](istream_view& in, node_type& n)
                               {
                                   in.deserialize(n.value)
                                       .deserialize(n.indices);
                               });
            }
        }

        using indices_container_type = decltype(t.free_slot_indices());
        in.deserialize<indices_container_type, traits>(t.free_slot_indices());
    }
};

} // namespace flow
