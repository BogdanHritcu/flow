#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <tuple>

#include <glm/vec2.hpp>

#include "binding.hpp"
#include "binding_enums.hpp"

namespace flow {

class input_context
{
private:
    template<concepts::binding_code T>
    struct typed_binding
    {
        binding value;
    };

    template<concepts::binding_code T>
    struct typed_size
    {
        std::size_t value;
    };

    template<concepts::binding_code T>
    using binding_code_state_array = std::array<binding_action_code,
                                                detail::remove_binding_code_flag(detail::binding_code_non_any_max<T>) + 1>;

    using typed_binding_tuple = std::tuple<typed_binding<binding_code>,
                                           typed_binding<key_code>,
                                           typed_binding<mouse_code>>;

    using binding_code_tuple = std::tuple<binding_code,
                                          key_code,
                                          mouse_code>;

    using typed_size_tuple = std::tuple<typed_size<key_code>,
                                        typed_size<mouse_code>>;

    using binding_code_state_array_tuple = std::tuple<binding_code_state_array<key_code>,
                                                      binding_code_state_array<mouse_code>>;

    binding_code_state_array_tuple m_binding_code_states{};

public:
    template<concepts::binding_code T>
    [[nodiscard]] constexpr binding& get_last_triggered() noexcept
    {
        return std::get<typed_binding<T>>(m_last_triggered_tuple).value;
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const binding& get_last_triggered() const noexcept
    {
        return std::get<typed_binding<T>>(m_last_triggered_tuple).value;
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr T& get_last_pressed() noexcept
    {
        return std::get<T>(m_last_pressed_tuple);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const T& get_last_pressed() const noexcept
    {
        return std::get<T>(m_last_pressed_tuple);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr T& get_last_released() noexcept
    {
        return std::get<T>(m_last_released_tuple);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const T& get_last_released() const noexcept
    {
        return std::get<T>(m_last_released_tuple);
    }

    template<concepts::binding_code T>
        requires(!std::same_as<T, binding_code>)
    [[nodiscard]] constexpr std::size_t& get_pressed_count() noexcept
    {
        return std::get<typed_size<T>>(m_pressed_count_tuple).value;
    }

    template<concepts::binding_code T>
        requires(!std::same_as<T, binding_code>)
    [[nodiscard]] constexpr const std::size_t& get_pressed_count() const noexcept
    {
        return std::get<typed_size<T>>(m_pressed_count_tuple).value;
    }

    template<std::same_as<binding_code> T>
    [[nodiscard]] constexpr const std::size_t get_pressed_count() const noexcept
    {
        return get_pressed_count<key_code>()
             + get_pressed_count<mouse_code>();
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr binding_code_state_array<T>& get_binding_code_states() noexcept
    {
        return std::get<binding_code_state_array<T>>(m_binding_code_states);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const binding_code_state_array<T>& get_binding_code_states() const noexcept
    {
        return std::get<binding_code_state_array<T>>(m_binding_code_states);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr binding_action_code& get_binding_code_state(T code) noexcept
    {
        return get_binding_code_states<T>()[detail::remove_binding_code_flag(code)];
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr const binding_action_code& get_binding_code_state(T code) const noexcept
    {
        return get_binding_code_states<T>()[detail::remove_binding_code_flag(code)];
    }

    [[nodiscard]] constexpr glm::vec2& get_cursor_position() noexcept
    {
        return m_cursor_position;
    }

    [[nodiscard]] constexpr const glm::vec2& get_cursor_position() const noexcept
    {
        return m_cursor_position;
    }

    [[nodiscard]] constexpr glm::vec2& get_previous_cursor_position() noexcept
    {
        return m_previous_cursor_position;
    }

    [[nodiscard]] constexpr const glm::vec2& get_previous_cursor_position() const noexcept
    {
        return m_previous_cursor_position;
    }

    template<concepts::binding_code T>
        requires(!std::same_as<T, binding_code>)
    constexpr void set_input_context_state(binding bind) noexcept
    {
        get_binding_code_state(bind.code_as<T>()) = bind.action();

        get_last_triggered<binding_code>() = bind;
        get_last_triggered<T>() = bind;

        if (bind.action() == binding_action_code::press)
        {
            get_last_pressed<binding_code>() = bind.code();
            get_last_pressed<T>() = bind.code_as<T>();
            ++get_pressed_count<T>();
        }
        else if (bind.action() == binding_action_code::release)
        {
            get_last_released<binding_code>() = bind.code();
            get_last_released<T>() = bind.code_as<T>();
            --get_pressed_count<T>();
        }
    }

    constexpr void set_cursor_state(const glm::vec2& position) noexcept
    {
        m_previous_cursor_position = m_cursor_position;
        m_cursor_position = position;
    }

    template<concepts::binding_code T>
        requires(!std::same_as<T, binding_code>)
    constexpr void reset_input_context_state() noexcept
    {
        get_binding_code_states<T>() = {};

        if (get_last_triggered<binding_code>().is<T>())
        {
            get_last_triggered<binding_code>() = {};
        }
        get_last_triggered<T>() = {};

        if (detail::is_binding_code_type<T>(get_last_pressed<binding_code>()))
        {
            get_last_pressed<binding_code>() = {};
        }
        get_last_pressed<T>() = {};

        if (detail::is_binding_code_type<T>(get_last_released<binding_code>()))
        {
            get_last_released<binding_code>() = {};
        }
        get_last_released<T>() = {};
    }

    constexpr void reset_cursor_state() noexcept
    {
        m_previous_cursor_position = {};
        m_cursor_position = {};
    }

private:
    typed_binding_tuple m_last_triggered_tuple{};
    binding_code_tuple m_last_pressed_tuple{};
    binding_code_tuple m_last_released_tuple{};
    typed_size_tuple m_pressed_count_tuple{};
    binding_code_state_array_tuple m_binding_code_state_tuple{};

    glm::vec2 m_previous_cursor_position{};
    glm::vec2 m_cursor_position{};
};

} // namespace flow
