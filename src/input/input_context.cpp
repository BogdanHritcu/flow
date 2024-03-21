#include "../../include/flow/input/input_context.hpp"

namespace flow {

input_context::input_context()
    : m_callbacks{}
    , m_bindings{}
    , m_actions{}
    , m_active{ false }
    , m_blocking_mode{ blocking_mode::on_action }
{}

void input_context::set_active(bool value) noexcept
{
    m_active = value;
}

void input_context::set_blocking_mode(blocking_mode mode) noexcept
{
    m_blocking_mode = mode;
}

void input_context::set_callback(const action& action, const input_event_callback_type& callback)
{
    m_callbacks.insert_or_assign(action, callback);
}

void input_context::add_binding(const action& action, const input_binding& binding)
{
    m_bindings[action].insert(binding);
    m_actions[binding].insert(action);
}

void input_context::remove_binding(const action& action, const input_binding& binding)
{
    if (auto it = m_bindings.find(action); it != m_bindings.end() && it->second.contains(binding))
    {
        it->second.erase(binding);
    }

    if (auto it = m_actions.find(binding); it != m_actions.end() && it->second.contains(action))
    {
        it->second.erase(action);
    }
}

void input_context::change_binding(const action& action, const input_binding& old_binding, const input_binding& new_binding)
{
    remove_binding(action, old_binding);
    add_binding(action, new_binding);
}

const input_event_callback_type& input_context::get_callback(const action& action)
{
    return m_callbacks[action];
}

const input_context::set_type<input_binding>& input_context::get_bindings(const action& action)
{
    return m_bindings[action];
}

const input_context::set_type<action>& input_context::get_actions(const input_binding& binding)
{
    return m_actions[binding];
}

bool input_context::is_active() const noexcept
{
    return m_active;
}

input_context::blocking_mode input_context::get_blocking_mode() const noexcept
{
    return m_blocking_mode;
}

bool input_context::handle(const input_event& event)
{
    return handle(event, m_actions[event.binding]);
}

bool input_context::handle(const input_event& event, const set_type<action>& actions)
{
    bool handled{ false };

    for (auto& action : actions)
    {
        if (auto it = m_callbacks.find(action); it != m_callbacks.end() && it->second)
        {
            it->second(event);
            handled = true;
        }
    }

    return m_blocking_mode != blocking_mode::never && (m_blocking_mode == blocking_mode::always || (m_blocking_mode == blocking_mode::on_action && handled));
}

} // namespace flow
