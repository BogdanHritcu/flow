#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "sized_int.hpp"

namespace flow {

template<std::integral T, std::size_t FractionBitCount>
    requires concepts::doubled_width_integral<T>
          && (std::numeric_limits<T>::digits >= FractionBitCount)
class basic_fixed_point
{
public:
    using underlying_type = T;
    static constexpr std::size_t fraction_bit_count = FractionBitCount;

public:
    constexpr basic_fixed_point() noexcept = default;

    template<std::integral U>
    constexpr basic_fixed_point(U value) noexcept
        : m_value{ static_cast<underlying_type>(value << fraction_bit_count) }
    {}

    template<std::floating_point U>
    constexpr basic_fixed_point(U value) noexcept
        : m_value{ static_cast<underlying_type>(value * static_cast<U>(std::make_unsigned_t<underlying_type>{ 1 } << fraction_bit_count)) }
    {}

    template<std::integral U>
    [[nodiscard]] explicit constexpr operator U() const noexcept
    {
        return static_cast<U>(m_value >> fraction_bit_count);
    }

    template<std::floating_point U>
    [[nodiscard]] explicit constexpr operator U() const noexcept
    {
        return static_cast<U>(m_value) / static_cast<U>(std::make_unsigned_t<underlying_type>{ 1 } << fraction_bit_count);
    }

    constexpr basic_fixed_point& operator+=(basic_fixed_point other) noexcept
    {
        m_value += other.m_value;

        return *this;
    }

    constexpr basic_fixed_point& operator-=(basic_fixed_point other) noexcept
    {
        m_value -= other.m_value;

        return *this;
    }

    constexpr basic_fixed_point& operator*=(basic_fixed_point other) noexcept
    {
        using dw_integral = make_doubled_width_integral_t<underlying_type>;

        m_value = static_cast<underlying_type>((static_cast<dw_integral>(m_value)
                                                * static_cast<dw_integral>(other.m_value))
                                               >> fraction_bit_count);

        return *this;
    }

    constexpr basic_fixed_point& operator/=(basic_fixed_point other) noexcept
    {
        using dw_integral = make_doubled_width_integral_t<underlying_type>;

        m_value = static_cast<underlying_type>((static_cast<dw_integral>(m_value) << fraction_bit_count)
                                               / static_cast<dw_integral>(other.m_value));

        return *this;
    }

    [[nodiscard]] friend constexpr basic_fixed_point operator+(basic_fixed_point lhs, basic_fixed_point rhs) noexcept
    {
        return lhs += rhs;
    }

    [[nodiscard]] friend constexpr basic_fixed_point operator-(basic_fixed_point lhs, basic_fixed_point rhs) noexcept
    {
        return lhs -= rhs;
    }

    [[nodiscard]] friend constexpr basic_fixed_point operator*(basic_fixed_point lhs, basic_fixed_point rhs) noexcept
    {
        return lhs *= rhs;
    }

    [[nodiscard]] friend constexpr basic_fixed_point operator/(basic_fixed_point lhs, basic_fixed_point rhs) noexcept
    {
        return lhs /= rhs;
    }

    [[nodiscard]] friend constexpr basic_fixed_point operator-(basic_fixed_point num) noexcept
    {
        num.m_value = -num.m_value;

        return num;
    }

    [[nodiscard]] friend constexpr auto operator<=>(basic_fixed_point lhs, basic_fixed_point rhs) noexcept = default;

    [[nodiscard]] constexpr operator bool() const noexcept
    {
        return m_value != 0;
    }

private:
    underlying_type m_value;
};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

using fixed24_8_t = basic_fixed_point<std::int32_t, 8>;
using fixed22_10_t = basic_fixed_point<std::int32_t, 10>;
using fixed20_12_t = basic_fixed_point<std::int32_t, 12>;
using fixed18_14_t = basic_fixed_point<std::int32_t, 14>;
using fixed16_16_t = basic_fixed_point<std::int32_t, 16>;
using fixed32_t = fixed16_16_t;

using ufixed24_8_t = basic_fixed_point<std::uint32_t, 8>;
using ufixed22_10_t = basic_fixed_point<std::uint32_t, 10>;
using ufixed20_12_t = basic_fixed_point<std::uint32_t, 12>;
using ufixed18_14_t = basic_fixed_point<std::uint32_t, 14>;
using ufixed16_16_t = basic_fixed_point<std::uint32_t, 16>;
using ufixed32_t = ufixed16_16_t;

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

} // namespace flow
