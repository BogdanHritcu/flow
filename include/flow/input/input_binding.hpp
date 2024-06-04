#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>

#include "input_enums.hpp"

namespace flow {

class input_binding
{
public:
    constexpr input_binding() noexcept = default;

    template<concepts::input_code T>
    constexpr input_binding(T code,
                            input_action_code action,
                            input_modifier_code mod) noexcept
        : m_code{ static_cast<detail::input_code_underlying_type>(code) }
        , m_action{ action }
        , m_mod{ mod }
    {}

    template<concepts::input_code T>
    constexpr input_binding(T code,
                            input_modifier_code mod,
                            input_action_code action) noexcept
        : input_binding(code, action, mod)
    {}

    template<concepts::input_code T>
    constexpr input_binding(T code, input_action_code action) noexcept
        : input_binding(code,
                        action,
                        input_modifier_code::none)
    {}

    template<concepts::input_code T>
    constexpr input_binding(T code, input_modifier_code mod) noexcept
        : input_binding(code,
                        input_action_code::press,
                        mod)
    {}

    template<concepts::input_code T>
    constexpr input_binding(T code) noexcept
        : input_binding(code,
                        input_action_code::press,
                        input_modifier_code::none)
    {}

    [[nodiscard]] constexpr input_code code() const noexcept
    {
        return m_code;
    }

    [[nodiscard]] constexpr input_action_code action() const noexcept
    {
        return m_action;
    }

    [[nodiscard]] constexpr input_modifier_code mod() const noexcept
    {
        return m_mod;
    }

    template<concepts::input_code T>
    [[nodiscard]] constexpr bool is() const noexcept
    {
        return detail::is_code_type<T>(m_code);
    }

    template<concepts::input_code T>
    [[nodiscard]] constexpr bool is_any() const noexcept
    {
        return detail::is_code_any<T>(m_code);
    }

    [[nodiscard]] friend constexpr bool
    operator==(input_binding, input_binding) noexcept = default;

private:
    input_code m_code{};
    input_action_code m_action{};
    input_modifier_code m_mod{};
};

template<concepts::input_code T, typename... Args>
constexpr input_binding make_binding(T code, Args... args) noexcept
{
    return input_binding(code, std::forward<Args>(args)...);
}

} // namespace flow

template<>
struct std::hash<flow::input_binding>
{
    [[nodiscard]] constexpr std::size_t operator()(flow::input_binding bind) const noexcept
    {
        static_assert(sizeof(bind) == sizeof(std::uint32_t), "invalid flow::input_binding size");

        return static_cast<std::size_t>(std::bit_cast<std::uint32_t>(bind));
    }
};
