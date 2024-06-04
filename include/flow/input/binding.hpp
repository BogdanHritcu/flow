#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>

#include "binding_enums.hpp"

namespace flow {

class binding
{
public:
    constexpr binding() noexcept = default;

    template<concepts::binding_code T>
    constexpr binding(T code,
                      binding_action_code action,
                      binding_modifier_code mod) noexcept
        : m_code{ static_cast<detail::binding_code_underlying_type>(code) }
        , m_action{ action }
        , m_mod{ mod }
    {}

    template<concepts::binding_code T>
    constexpr binding(T code,
                      binding_modifier_code mod,
                      binding_action_code action) noexcept
        : binding(code, action, mod)
    {}

    template<concepts::binding_code T>
    constexpr binding(T code, binding_action_code action) noexcept
        : binding(code,
                  action,
                  binding_modifier_code::none)
    {}

    template<concepts::binding_code T>
    constexpr binding(T code, binding_modifier_code mod) noexcept
        : binding(code,
                  binding_action_code::press,
                  mod)
    {}

    template<concepts::binding_code T>
    constexpr binding(T code) noexcept
        : binding(code,
                  binding_action_code::press,
                  binding_modifier_code::none)
    {}

    [[nodiscard]] constexpr binding_code code() const noexcept
    {
        return m_code;
    }

    [[nodiscard]] constexpr binding_action_code action() const noexcept
    {
        return m_action;
    }

    [[nodiscard]] constexpr binding_modifier_code mod() const noexcept
    {
        return m_mod;
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr bool is() const noexcept
    {
        return detail::is_code_type<T>(m_code);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr bool is_any() const noexcept
    {
        return detail::is_code_any<T>(m_code);
    }

    [[nodiscard]] friend constexpr bool
    operator==(binding, binding) noexcept = default;

private:
    binding_code m_code{};
    binding_action_code m_action{};
    binding_modifier_code m_mod{};
};

template<concepts::binding_code T, typename... Args>
constexpr binding make_binding(T code, Args... args) noexcept
{
    return binding(code, std::forward<Args>(args)...);
}

} // namespace flow

template<>
struct std::hash<flow::binding>
{
    [[nodiscard]] constexpr std::size_t operator()(flow::binding bind) const noexcept
    {
        static_assert(sizeof(bind) == sizeof(std::uint32_t), "invalid flow::binding size");

        return static_cast<std::size_t>(std::bit_cast<std::uint32_t>(bind));
    }
};
