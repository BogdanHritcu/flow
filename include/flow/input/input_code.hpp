#pragma once

#include "../utility/concepts.hpp"
#include "../utility/helpers.hpp"
#include "input_enums.hpp"

namespace flow {

template<typename T>
struct input_code_meta
{};

class input_code
{
public:
    template<typename... Args>
    struct any
    {
        template<std::integral T>
        [[nodiscard]] explicit constexpr operator T() const noexcept
        {
            return static_cast<T>((input_code_meta<Args>::max_value | ...));
        }
    };

    template<>
    struct any<>
    {
        template<std::integral T>
        [[nodiscard]] explicit constexpr operator T() const noexcept
        {
            return static_cast<T>(any<key, mouse_button>{});
        }
    };

    constexpr input_code() noexcept
        : m_code{}
    {}

    template<typename T>
    constexpr input_code(const T& code) noexcept
        : m_code{ static_cast<std::uint32_t>(code) | input_code_meta<T>::flags }
    {}

    [[nodiscard]] constexpr bool operator==(const input_code&) const noexcept = default;

    template<std::integral T>
    [[nodiscard]] explicit constexpr operator T() const noexcept
    {
        return static_cast<T>(m_code);
    }

    template<typename... Args>
    [[nodiscard]] constexpr bool is_all() const noexcept
    {
        return is_all<Args...>(m_code);
    }

    template<typename... Args>
    [[nodiscard]] constexpr bool is_any() const noexcept
    {
        return is_any<Args...>(m_code);
    }

    template<typename... Args>
    [[nodiscard]] static constexpr bool is_all(const input_code& code) noexcept
    {
        return (((code.m_code & input_code_meta<Args>::flags) != 0) && ...);
    }

    template<typename... Args>
    [[nodiscard]] static constexpr bool is_any(const input_code& code) noexcept
    {
        return (((code.m_code & input_code_meta<Args>::flags) != 0) || ...);
    }

private:
    std::uint32_t m_code;
};

template<>
struct input_code_meta<key>
{
    static constexpr std::uint32_t flags{ 1 << 10 };
    static constexpr std::uint32_t max_value{ static_cast<std::uint32_t>(key::Menu) + 1 };
};

template<>
struct input_code_meta<mouse_button>
{
    static constexpr std::uint32_t flags{ 1 << 11 };
    static constexpr std::uint32_t max_value{ static_cast<std::uint32_t>(mouse_button::B8) + 1 };
};

template<typename... Args>
struct input_code_meta<input_code::any<Args...>>
{
    static constexpr std::uint32_t flags{ (static_cast<std::uint32_t>(input_code_meta<Args>::flags) | ...) };
    static constexpr std::uint32_t max_value{ (static_cast<std::uint32_t>(input_code_meta<Args>::max_value) | ...) };
};

} // namespace flow

template<>
struct std::hash<flow::input_code>
{
    std::size_t operator()(const flow::input_code& code) const noexcept
    {
        return std::hash<std::uint32_t>{}(static_cast<std::uint32_t>(code));
    }
};
