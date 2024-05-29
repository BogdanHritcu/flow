#pragma once

#include <bit>
#include <concepts>
#include <limits>

#include "../core/assertion.hpp"

namespace flow {

template<std::unsigned_integral T>
[[nodiscard]] constexpr T fast_log2(T n)
{
    FLOW_ASSERT(n != 0, "log argument cannot be 0");
    return std::numeric_limits<T>::digits - std::countl_zero(n) - static_cast<T>(1);
}

template<typename T, std::floating_point F>
[[nodiscard]] constexpr T lerp(T x, T y, F t) noexcept
{
    return (F{ 1 } - t) * x + t * y;
};

template<std::unsigned_integral T>
[[nodiscard]] constexpr T add_sat(T a, T b) noexcept
{
    return a <= std::numeric_limits<T>::max() - b
             ? a + b
             : std::numeric_limits<T>::max();
}

template<std::unsigned_integral T>
[[nodiscard]] constexpr T sub_sat(T a, T b) noexcept
{
    return a >= std::numeric_limits<T>::min() + b
             ? a - b
             : std::numeric_limits<T>::min();
}

} // namespace flow
