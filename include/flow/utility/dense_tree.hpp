#pragma once

#include <concepts>
#include <limits>
#include <vector>

#include "concepts.hpp"
#include "istream.hpp"
#include "ostream.hpp"
#include "traits.hpp"

namespace flow {

template<typename DataT, std::unsigned_integral IndexT>
class dense_tree
{
public:
    using data_type = DataT;
    using index_type = IndexT;

    static constexpr auto nullindex = std::numeric_limits<index_type>::max();

    struct node_indices
    {
        index_type parent;
        index_type first_child;
        index_type next_sibling;
    };

private:
    struct node_data_first
    {
        data_type data;
        node_indices indices;
    };

    struct node_data_last
    {
        node_indices indices;
        data_type data;
    };

public:
    using node_type = min_size_t<node_data_first, node_data_last>;

    [[nodiscard]] static constexpr bool node_is_valid(const node_type& node, bool is_root) noexcept
    {
        return (is_root && node.indices.parent == nullindex && node.indices.next_sibling == nullindex)
            || (!is_root && node.indices.parent != nullindex);
    }

    static constexpr void node_invalidate(node_type& node) noexcept
    {
        // root node can't have non null index as sibling
        // other nodes can't have null index as parent
        node.indices.next_sibling = ~nullindex;
        node.indices.parent = nullindex;
    }

    // clang-format off

    template<typename... Args>
    static constexpr node_type make_node(index_type parent_index,
                                         index_type first_child_index,
                                         index_type next_sibling_index,
                                         Args&&... args)
        noexcept(noexcept(std::is_nothrow_constructible_v<data_type, Args...>))
    {
        if constexpr (std::same_as<node_type, node_data_first>)
        {
            return node_type{
                .data = { std::forward<Args>(args)... },
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
                .data = { std::forward<Args>(args)... }
            };
        }
    }

    // clang-format on

    // clang-format off

    template<typename... Args>
    static constexpr node_type make_node(index_type parent_index, Args&&... args)
        noexcept(noexcept(std::is_nothrow_constructible_v<data_type, Args...>))
    {
        return make_node(parent_index,
                         nullindex,
                         nullindex,
                         std::forward<Args>(args)...);
    }

    // clang-format on

public:
    constexpr dense_tree() noexcept
        : m_node_slots{}
        , m_free_slot_indices{}
    {}

    constexpr dense_tree(const data_type& data)
        : m_node_slots{ make_node(nullindex, data) }
        , m_free_slot_indices{}
    {}

    constexpr void set_root(const data_type& data)
    {
        if (is_node(0))
        {
            m_node_slots[0].data = data;
        }
        else
        {
            m_node_slots.push_back(make_node(nullindex, data));
        }
    }

    constexpr void insert(index_type parent_index, const data_type& data)
    {
        if (!is_node(parent_index))
        {
            return;
        }

        if (m_free_slot_indices.size() > 0)
        {
            const index_type child_index = m_free_slot_indices.back();
            m_free_slot_indices.pop_back();

            if (child_index < m_node_slots.size()
                && !node_is_valid(m_node_slots[child_index], child_index == 0))
            {
                m_node_slots[child_index] = make_node(parent_index,
                                                      nullindex,
                                                      m_node_slots[parent_index].indices.first_child,
                                                      data);

                m_node_slots[parent_index].indices.first_child = child_index;

                return;
            }
        }

        m_node_slots.push_back(make_node(parent_index,
                                         nullindex,
                                         m_node_slots[parent_index].indices.first_child,
                                         data));

        m_node_slots[parent_index].indices.first_child = static_cast<index_type>(m_node_slots.size() - 1);
    }

    constexpr void erase(index_type node_index)
    {
        if (!is_node(node_index))
        {
            return;
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

        queue_push(node_index);

        while (queue_size() > 0)
        {
            auto current_node_index = queue_pop();

            // no need to check if the node was visited, because
            // we do it before pushing them in the queue
            // a node that is invalid corresponds to a visited node

            auto child_index = m_node_slots[current_node_index].indices.first_child;

            while (is_node(child_index))
            {
                queue_push(child_index);
                child_index = m_node_slots[child_index].indices.next_sibling;
            }

            node_invalidate(m_node_slots[current_node_index]);
        }
    }

    [[nodiscard]] constexpr bool is_node(index_type node_index) const noexcept
    {
        return node_index < m_node_slots.size()
            && node_is_valid(m_node_slots[node_index], node_index == 0);
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
    std::vector<node_type> m_node_slots;
    std::vector<index_type> m_free_slot_indices;
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
                                  out.serialize(n.data)
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
                                   in.deserialize(n.data)
                                       .deserialize(n.indices);
                               });
            }
        }

        using indices_container_type = decltype(t.free_slot_indices());
        in.deserialize<indices_container_type, traits>(t.free_slot_indices());
    }
};

} // namespace flow
