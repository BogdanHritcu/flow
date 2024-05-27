#pragma once

#include <optional>
#include <unordered_map>

#include "input_binding.hpp"

namespace flow {

template<typename CallbackT, typename IndexT>
class input_context
{
public:
    using callback_type = CallbackT;
    using index_type = IndexT;

private:
    using callback_index_map_type = std::unordered_map<input_binding, index_type>;

public:
    [[nodiscard]] constexpr bool has_binding(input_binding bind) const
    {
        return m_callback_index_map.contains(bind);
    }

    constexpr void remove_binding(input_binding bind)
    {
        auto it = m_callback_index_map.find(bind);

        if (it == m_callback_index_map.end())
        {
            return;
        }

        m_callback_index_map.erase(bind);
    }

    constexpr void set_callback_index(input_binding bind, index_type index)
    {
        m_callback_index_map[bind] = index;
    }

    [[nodiscard]] constexpr std::optional<index_type> get_callback_index(input_binding bind) const
    {
        auto it = m_callback_index_map.find(bind);

        return (it != m_callback_index_map.end() ? std::make_optional(it->second) : std::nullopt);
    }

    [[nodiscard]] constexpr std::vector<input_binding> get_bindings(index_type callback_index) const
    {
        std::vector<input_binding> bindings{};

        for (const auto& [bind, index] : m_callback_index_map)
        {
            if (index == callback_index)
            {
                bindings.emplace_back(bind);
            }
        }

        return bindings;
    }

private:
    callback_index_map_type m_callback_index_map{};
};

} // namespace flow
