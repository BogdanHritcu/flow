#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include "../utility/helpers.hpp"
#include "input_binding.hpp"
#include "input_context.hpp"
#include "input_enums.hpp"

namespace flow {

enum class fallthrough_mode
{
    never,
    always,
    match,
    no_match
};

class engine_interface;

class input_system
{
public:
    using index_type = std::uint64_t;
    using callback_type = std::function<void(engine_interface, input_binding)>;
    using context_type = input_context<callback_type, index_type>;

public:
    bool register_callback(std::string_view name, const callback_type& callback)
    {
        const auto callback_index = static_cast<index_type>(m_callbacks.size());
        auto [it, is_inserted] = m_callback_name_map.emplace(name, callback_index);

        if (is_inserted)
        {
            m_callbacks.emplace_back(callback);
        }

        return is_inserted;
    }

    void unregister_callback(std::string_view name)
    {
        auto it = m_callback_name_map.find(name);
        if (it == m_callback_name_map.end())
        {
            return;
        }

        m_callback_name_map.erase(it);
    }

    bool register_context(std::string_view name)
    {
        auto context_index = static_cast<index_type>(m_contexts.size());
        auto [it, is_inserted] = m_context_name_map.emplace(name, context_index);

        if (is_inserted)
        {
            m_contexts.emplace_back(); // empty input_context
        }

        return is_inserted;
    }

    void unregister_context(std::string_view name)
    {
        auto it = m_context_name_map.find(name);
        if (it == m_context_name_map.end())
        {
            return;
        }

        m_context_name_map.erase(it);
    }

    bool register_binding(input_binding bind,
                          std::string_view callback_name,
                          std::string_view context_name)
    {
        auto callback_index = get_callback_index(callback_name);
        if (!callback_index.has_value())
        {
            return false;
        }

        auto context_index = get_context_index(context_name);
        if (!context_index.has_value())
        {
            return false;
        }

        m_contexts[*context_index].set_callback_index(bind, *callback_index);

        return true;
    }

    void unregister_binding(input_binding bind, std::string_view context_name)
    {
        auto context_index = get_context_index(context_name);
        if (!context_index.has_value())
        {
            return;
        }

        m_contexts[*context_index].remove_binding(bind);
    }

    bool push_context(std::string_view name, void* user_ptr, fallthrough_mode fallthrough)
    {
        auto it = m_context_name_map.find(name);
        auto context_index = it->second;

        if (it == m_context_name_map.end() || context_index >= m_contexts.size())
        {
            return false;
        }

        m_context_handle_stack.emplace_back(user_ptr, context_index, fallthrough);

        return true;
    }

    void pop_context()
    {
        if (m_context_handle_stack.size() > 0)
        {
            m_context_handle_stack.pop_back();
        }
    }

    [[nodiscard]] bool callback_exists(std::string_view name) const
    {
        return get_callback_index(name).has_value();
    }

    [[nodiscard]] bool context_exists(std::string_view name) const
    {
        return get_context_index(name).has_value();
    }

    [[nodiscard]] bool binding_exists(input_binding bind, std::string_view context_name) const
    {
        auto index = get_context_index(context_name);

        return (index.has_value() && m_contexts[*index].has_binding(bind));
    }

    [[nodiscard]] std::vector<input_binding> get_bindings(std::string_view callback_name,
                                                          std::string_view context_name) const
    {
        auto callback_index = get_callback_index(callback_name);

        if (!callback_index.has_value())
        {
            return {};
        }

        auto context_index = get_context_index(context_name);

        if (!context_index.has_value())
        {
            return {};
        }

        return m_contexts[*context_index].get_bindings(*callback_index);
    }

    template<typename... Args>
    void invoke_callbacks(input_binding bind, Args... args) const
    {
        auto invoke_if_match = [&](index_type stack_index) -> bool
        {
            context_handle handle = m_context_handle_stack[stack_index];
            auto callback_index = m_contexts[handle.index].get_callback_index(bind);
            bool match = callback_index.has_value() && *callback_index < m_callbacks.size();

            if (!match)
            {
                auto any_bind = make_binding(detail::to_any_code(bind.code()),
                                             bind.action(),
                                             bind.mod());
                callback_index = m_contexts[handle.index].get_callback_index(any_bind);
                match = callback_index.has_value() && *callback_index < m_callbacks.size();
            }

            if (!match)
            {
                auto any_bind = make_binding(input_code::any,
                                             bind.action(),
                                             bind.mod());
                callback_index = m_contexts[handle.index].get_callback_index(any_bind);
                match = callback_index.has_value() && *callback_index < m_callbacks.size();
            }

            if (match)
            {
                std::invoke(m_callbacks[*callback_index], std::forward<Args>(args)..., bind);
            }

            return (handle.fallthrough == fallthrough_mode::match && match)
                || (handle.fallthrough == fallthrough_mode::no_match && !match)
                || (handle.fallthrough == fallthrough_mode::always);
        };

        auto stack_index = m_context_handle_stack.size();

        // invoke_if_match will return true if the callback is fallthrough
        while (stack_index > 0 && invoke_if_match(--stack_index))
        {
            // do nothing
        }
    }

private:
    [[nodiscard]] std::optional<index_type> get_callback_index(std::string_view name) const
    {
        auto it = m_callback_name_map.find(name);

        return (it != m_callback_name_map.end()
                && it->second < m_callbacks.size())
                 ? std::make_optional(it->second)
                 : std::nullopt;
    }

    [[nodiscard]] std::optional<index_type> get_context_index(std::string_view name) const
    {
        auto it = m_context_name_map.find(name);

        return (it != m_context_name_map.end()
                && it->second < m_contexts.size())
                 ? std::make_optional(it->second)
                 : std::nullopt;
    }

private:
    struct context_handle
    {
        void* user_ptr;
        index_type index;
        fallthrough_mode fallthrough;
    };

    std::vector<callback_type> m_callbacks;
    unordered_string_map<index_type> m_callback_name_map;

    std::vector<context_type> m_contexts;
    unordered_string_map<index_type> m_context_name_map;

    std::vector<context_handle> m_context_handle_stack;
};

} // namespace flow
