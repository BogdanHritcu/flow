#pragma once

#include <cstddef>
#include <type_traits>

namespace flow {

template<template<typename> typename T, typename U, std::ptrdiff_t Exponent>
    requires(Exponent != 0)
struct unit
{
    using value_type = U;

    constexpr unit() noexcept
        : value{}
    {}

    template<std::convertible_to<U> V>
    constexpr explicit unit(const V& val) noexcept
        : value{ static_cast<U>(val) }
    {}

    template<std::convertible_to<U> V>
    constexpr explicit unit(const unit<T, V, Exponent>& unit) noexcept
        : value{ static_cast<U>(unit.value) }
    {}

    template<std::convertible_to<U> V>
    [[nodiscard]] constexpr explicit operator V() noexcept
    {
        return static_cast<V>(value);
    }

    U value;
};

namespace concepts {

    template<typename T>
    concept unit_has_add = static_cast<bool>(T::enable_add);

    template<typename T>
    concept unit_has_sub = static_cast<bool>(T::enable_sub);

    template<typename T>
    concept unit_has_mul = static_cast<bool>(T::enable_mul);

    template<typename T>
    concept unit_has_mul_s = static_cast<bool>(T::enable_mul_s);

    template<typename T>
    concept unit_has_div = static_cast<bool>(T::enable_div);

    template<typename T>
    concept unit_has_div_s = static_cast<bool>(T::enable_div_s);

} // namespace concepts

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t Exponent>
    requires(concepts::unit_has_add<T<U>> && concepts::unit_has_add<T<V>>)
[[nodiscard]] constexpr auto operator+(const unit<T, U, Exponent>& lhs,
                                       const unit<T, V, Exponent>& rhs)
{
    return unit<T, std::common_type_t<U, V>, Exponent>{ lhs.value + rhs.value };
}

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t Exponent>
    requires(concepts::unit_has_sub<T<U>> && concepts::unit_has_sub<T<V>>)
[[nodiscard]] constexpr auto operator-(const unit<T, U, Exponent>& lhs,
                                       const unit<T, V, Exponent>& rhs)
{
    return unit<T, std::common_type_t<U, V>, Exponent>{ lhs.value - rhs.value };
}

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t ExponentU, std::ptrdiff_t ExponentV>
    requires(concepts::unit_has_mul<T<U>> && concepts::unit_has_mul<T<V>> && (ExponentU + ExponentV != 0))
[[nodiscard]] constexpr auto operator*(const unit<T, U, ExponentU>& lhs,
                                       const unit<T, V, ExponentV>& rhs)
{
    return unit<T, std::common_type_t<U, V>, ExponentU + ExponentV>{ lhs.value * rhs.value };
}

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t ExponentU, std::ptrdiff_t ExponentV>
    requires(concepts::unit_has_mul<T<U>> && concepts::unit_has_mul<T<V>> && (ExponentU + ExponentV == 0))
[[nodiscard]] constexpr auto operator*(const unit<T, U, ExponentU>& lhs,
                                       const unit<T, V, ExponentV>& rhs)
{
    return std::common_type_t<U, V>{ lhs.value * rhs.value };
}

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t Exponent>
    requires(concepts::unit_has_mul_s<T<U>>)
[[nodiscard]] constexpr auto operator*(const unit<T, U, Exponent>& lhs,
                                       const V& rhs)
{
    return unit<T, std::common_type_t<U, V>, Exponent>{ lhs.value * rhs };
}

template<typename U, template<typename> typename T, typename V, std::ptrdiff_t Exponent>
    requires(concepts::unit_has_mul_s<T<V>>)
[[nodiscard]] constexpr auto operator*(const U& lhs,
                                       const unit<T, V, Exponent>& rhs)
{
    return unit<T, std::common_type_t<U, V>, Exponent>{ lhs * rhs.value };
}

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t ExponentU, std::ptrdiff_t ExponentV>
    requires(concepts::unit_has_div<T<U>> && concepts::unit_has_div<T<V>> && (ExponentU - ExponentV != 0))
[[nodiscard]] constexpr auto operator/(const unit<T, U, ExponentU>& lhs,
                                       const unit<T, V, ExponentV>& rhs)
{
    return unit<T, std::common_type_t<U, V>, ExponentU - ExponentV>{ lhs.value / rhs.value };
}

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t ExponentU, std::ptrdiff_t ExponentV>
    requires(concepts::unit_has_div<T<U>> && concepts::unit_has_div<T<V>> && (ExponentU - ExponentV == 0))
[[nodiscard]] constexpr auto operator/(const unit<T, U, ExponentU>& lhs,
                                       const unit<T, V, ExponentV>& rhs)
{
    return std::common_type_t<U, V>{ lhs.value / rhs.value };
}

template<template<typename> typename T, typename U, typename V, std::ptrdiff_t Exponent>
    requires(concepts::unit_has_div_s<T<U>>)
[[nodiscard]] constexpr auto operator/(const unit<T, U, Exponent>& lhs,
                                       const V& rhs)
{
    return unit<T, std::common_type_t<U, V>, Exponent>{ lhs.value / rhs };
}

template<typename U, template<typename> typename T, typename V, std::ptrdiff_t Exponent>
    requires(concepts::unit_has_div_s<T<V>>)
[[nodiscard]] constexpr auto operator/(const U& lhs,
                                       const unit<T, V, Exponent>& rhs)
{
    return unit<T, std::common_type_t<U, V>, -Exponent>{ lhs / rhs.value };
}

} // namespace flow
