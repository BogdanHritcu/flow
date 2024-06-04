#pragma once

#include <string_view>

#include "../input/binding.hpp"
#include "../input/binding_enums.hpp"
#include "input_system.hpp"

namespace flow {

class input_system_interface
{
public:
    using index_type = input_system::index_type;
    using binding_callback_type = input_system::binding_callback_type;

    // clang-format off

    static constexpr struct { using enum key_code;            } key;
    static constexpr struct { using enum mouse_code;           } mouse;
    static constexpr struct { using enum binding_action_code;   } action;
    static constexpr struct { using enum binding_modifier_code; } mod;

    // clang-format on

    static constexpr auto any = binding_code::any;

public:
    constexpr input_system_interface() noexcept = default;

    constexpr input_system_interface(input_system& input_system) noexcept
        : m_input_system{ &input_system }
    {}

    bool register_binding_callback(std::string_view name,
                                   const binding_callback_type& callback)
    {
        return m_input_system->register_binding_callback(name, callback);
    }

    void unregister_binding_callback(std::string_view name)
    {
        m_input_system->unregister_binding_callback(name);
    }

    bool register_binding_context(std::string_view name)
    {
        return m_input_system->register_binding_context(name);
    }

    void unregister_binding_context(std::string_view name)
    {
        m_input_system->unregister_binding_context(name);
    }

    bool register_binding(binding bind,
                          std::string_view callback_name,
                          std::string_view context_name)
    {
        return m_input_system->register_binding(bind, callback_name, context_name);
    }

    void unregister_binding(binding bind, std::string_view context_name)
    {
        m_input_system->unregister_binding(bind, context_name);
    }

    bool push_binding_context(std::string_view name,
                              void* user_ptr = nullptr,
                              fallthrough_mode fallthrough = fallthrough_mode::never)
    {
        return m_input_system->push_binding_context(name, user_ptr, fallthrough);
    }

    void pop_binding_context()
    {
        m_input_system->pop_binding_context();
    }

    [[nodiscard]] bool binding_callback_exists(std::string_view name) const
    {
        return m_input_system->binding_callback_exists(name);
    }

    [[nodiscard]] bool binding_context_exists(std::string_view name) const
    {
        return m_input_system->binding_context_exists(name);
    }

    [[nodiscard]] bool binding_exists(binding bind,
                                      std::string_view context_name) const
    {
        return m_input_system->binding_exists(bind, context_name);
    }

    [[nodiscard]] std::vector<binding> get_bindings(std::string_view callback_name,
                                                    std::string_view context_name)
    {
        return m_input_system->get_bindings(callback_name, context_name);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const binding& last_triggered() const noexcept
    {
        return m_input_system->context().get_last_triggered<T>();
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const T& last_pressed() const noexcept
    {
        return m_input_system->context().get_last_pressed<T>();
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const T& last_released() const noexcept
    {
        return m_input_system->context().get_last_released<T>();
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr std::size_t pressed_count() const noexcept
    {
        return m_input_system->context().get_pressed_count<T>();
    }

    [[nodiscard]] constexpr const glm::vec2& cursor_position() const noexcept
    {
        return m_input_system->context().get_cursor_position();
    }

    [[nodiscard]] constexpr const glm::vec2& previous_cursor_position() const noexcept
    {
        return m_input_system->context().get_previous_cursor_position();
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr bool is_pressed(T code) const noexcept
    {
        if constexpr (std::same_as<T, binding_code>)
        {
            return (detail::is_binding_code_any<binding_code>(code) && pressed_count<binding_code>() > 0)
                || (detail::is_binding_code_type<key_code>(code) && is_pressed(detail::binding_code_cast<key_code>(code)))
                || (detail::is_binding_code_type<mouse_code>(code) && is_pressed(detail::binding_code_cast<mouse_code>(code)));
        }
        else
        {
            return (detail::is_binding_code_any<T>(code) && pressed_count<T>() > 0)
                || m_input_system->context().get_binding_code_state(code) != binding_action_code::release;
        }
    }

private:
    input_system* m_input_system{};
};

} // namespace flow
