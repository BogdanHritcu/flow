#pragma once

#include <entt/container/dense_map.hpp>
#include <entt/container/dense_set.hpp>

#include "../core/action.hpp"
#include "input_binding.hpp"
#include "input_event.hpp"

namespace flow {

class input_context
{
public:
    template<typename K, typename V>
    using map_type = entt::dense_map<K, V>;
    template<typename T>
    using set_type = entt::dense_set<T>;

    enum class blocking_mode
    {
        on_action,
        always,
        never
    };

public:
    input_context();

    void set_active(bool value = true) noexcept;
    void set_blocking_mode(blocking_mode mode) noexcept;
    void set_callback(const action& action, const input_event_callback_type& callback);
    void add_binding(const action& action, const input_binding& binding);
    void remove_binding(const action& action, const input_binding& binding);
    void change_binding(const action& action, const input_binding& old_binding, const input_binding& new_binding);

    [[nodiscard]] const input_event_callback_type& get_callback(const action& action);
    [[nodiscard]] const set_type<input_binding>& get_bindings(const action& action);
    [[nodiscard]] const set_type<action>& get_actions(const input_binding& binding);
    [[nodiscard]] bool is_active() const noexcept;
    [[nodiscard]] blocking_mode get_blocking_mode() const noexcept;

private:
    bool handle(const input_event& event);
    bool handle(const input_event& event, const set_type<action>& actions);

    template<typename... Args>
    bool handle_any(const input_event& event)
    {
        return handle(event, m_actions[{ input_code::any<Args...>{}, event.binding.mod, event.binding.action }]);
    }

private:
    map_type<action, input_event_callback_type> m_callbacks;
    map_type<action, set_type<input_binding>> m_bindings;
    map_type<input_binding, set_type<action>> m_actions;
    bool m_active;
    blocking_mode m_blocking_mode;

    friend class application;
};

} // namespace flow
