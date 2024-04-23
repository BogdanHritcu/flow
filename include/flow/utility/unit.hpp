#pragma once

#include <cstddef>
#include <type_traits>

namespace flow {

namespace concepts {

    template<typename T>
    concept unit_add_enabled = !requires { T::disable_add; } || !static_cast<bool>(T::disable_add);

    template<typename T>
    concept unit_sub_enabled = !requires { T::disable_sub; } || !static_cast<bool>(T::disable_sub);

    template<typename T>
    concept unit_mul_enabled = !requires { T::disable_mul; } || !static_cast<bool>(T::disable_mul);

    template<typename T>
    concept unit_div_enabled = !requires { T::disable_div; } || !static_cast<bool>(T::disable_div);

    template<typename T>
    concept unit_cmp_enabled = !requires { T::disable_cmp; } || !static_cast<bool>(T::disable_cmp);

} // namespace concepts

template<template<typename> typename UnitT, typename ValueT, std::ptrdiff_t Exponent>
    requires(Exponent != 0)
struct unit
{
    using value_type = ValueT;

    ValueT value;

    constexpr unit() noexcept
        : value{}
    {}

    template<std::convertible_to<ValueT> T>
    constexpr explicit unit(const T& val) noexcept
        : value{ static_cast<ValueT>(val) }
    {}

    template<std::convertible_to<ValueT> T>
    constexpr explicit unit(const unit<UnitT, T, Exponent>& unit) noexcept
        : value{ static_cast<ValueT>(unit.value) }
    {}

    template<std::convertible_to<ValueT> T>
    [[nodiscard]] constexpr explicit operator T() noexcept
    {
        return static_cast<T>(value);
    }

    // increment/decrement operators

    constexpr unit& operator++() noexcept(noexcept(++value))
        requires(concepts::unit_add_enabled<UnitT<ValueT>>)
    {
        ++value;
        return *this;
    }

    constexpr unit operator++(int) noexcept(noexcept(value++))
        requires(concepts::unit_add_enabled<UnitT<ValueT>>)
    {
        return unit{ value++ };
    }

    constexpr unit& operator--() noexcept(noexcept(--value))
        requires(concepts::unit_sub_enabled<UnitT<ValueT>>)
    {
        --value;
        return *this;
    }

    constexpr unit operator--(int) noexcept(noexcept(value--))
        requires(concepts::unit_sub_enabled<UnitT<ValueT>>)
    {
        return unit{ value-- };
    }

    // compound assignment operators

    template<template<typename> typename T, typename U>
        requires(concepts::unit_add_enabled<UnitT<ValueT>> && concepts::unit_add_enabled<T<U>>)
    constexpr unit& operator+=(const unit<T, U, Exponent>& rhs) noexcept(noexcept(value += rhs.value))
    {
        value += rhs.value;
        return *this;
    }

    template<template<typename> typename T, typename U>
        requires(concepts::unit_sub_enabled<UnitT<ValueT>> && concepts::unit_sub_enabled<T<U>>)
    constexpr unit& operator-=(const unit<T, U, Exponent>& rhs) noexcept(noexcept(value -= rhs.value))
    {
        value -= rhs.value;
        return *this;
    }

    template<typename T>
        requires(concepts::unit_mul_enabled<UnitT<ValueT>>)
    constexpr unit& operator*=(const T& rhs) noexcept(noexcept(value *= rhs.value))
    {
        value *= rhs;
        return *this;
    }

    template<typename T>
        requires(concepts::unit_div_enabled<UnitT<ValueT>>)
    constexpr unit& operator/=(const T& rhs) noexcept(noexcept(value /= rhs.value))
    {
        value /= rhs;
        return *this;
    }

    // binary arithmetic operators

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t E>
        requires(concepts::unit_add_enabled<T<U>> && concepts::unit_add_enabled<T<V>>)
    [[nodiscard]] friend constexpr auto operator+(const unit<T, U, E>& lhs,
                                                  const unit<T, V, E>& rhs) noexcept(noexcept(lhs.value + rhs.value))
    {
        return unit<T, std::common_type_t<U, V>, E>{ lhs.value + rhs.value };
    }

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t E>
        requires(concepts::unit_sub_enabled<T<U>> && concepts::unit_sub_enabled<T<V>>)
    [[nodiscard]] friend constexpr auto operator-(const unit<T, U, E>& lhs,
                                                  const unit<T, V, E>& rhs) noexcept(noexcept(lhs.value - rhs.value))
    {
        return unit<T, std::common_type_t<U, V>, E>{ lhs.value - rhs.value };
    }

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t EU, std::ptrdiff_t EV>
        requires(concepts::unit_mul_enabled<T<U>> && concepts::unit_mul_enabled<T<V>>)
    [[nodiscard]] friend constexpr auto operator*(const unit<T, U, EU>& lhs,
                                                  const unit<T, V, EV>& rhs) noexcept(noexcept(lhs.value * rhs.value))
    {
        if constexpr (EU + EV != 0)
        {
            return unit<T, std::common_type_t<U, V>, EU + EV>{ lhs.value * rhs.value };
        }
        else
        {
            return std::common_type_t<U, V>{ lhs.value * rhs.value };
        }
    }

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t E>
        requires(concepts::unit_mul_enabled<T<U>>)
    [[nodiscard]] friend constexpr auto operator*(const unit<T, U, E>& lhs,
                                                  const V& rhs) noexcept(noexcept(lhs.value * rhs))
    {
        return unit<T, std::common_type_t<U, V>, E>{ lhs.value * rhs };
    }

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t E>
        requires(concepts::unit_mul_enabled<T<U>>)
    [[nodiscard]] friend constexpr auto operator*(const V& lhs,
                                                  const unit<T, U, E>& rhs) noexcept(noexcept(lhs * rhs.value))
    {
        return unit<T, std::common_type_t<U, V>, E>{ lhs * rhs.value };
    }

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t EU, std::ptrdiff_t EV>
        requires(concepts::unit_div_enabled<T<U>> && concepts::unit_div_enabled<T<V>>)
    [[nodiscard]] friend constexpr auto operator/(const unit<T, U, EU>& lhs,
                                                  const unit<T, V, EV>& rhs) noexcept(noexcept(lhs.value / rhs.value))
    {
        if constexpr (EU - EV != 0)
        {
            return unit<T, std::common_type_t<U, V>, EU - EV>{ lhs.value / rhs.value };
        }
        else
        {
            return std::common_type_t<U, V>{ lhs.value / rhs.value };
        }
    }

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t E>
        requires(concepts::unit_div_enabled<T<U>>)
    [[nodiscard]] friend constexpr auto operator/(const unit<T, U, E>& lhs,
                                                  const V& rhs) noexcept(noexcept(lhs.value / rhs))
    {
        return unit<T, std::common_type_t<U, V>, E>{ lhs.value / rhs };
    }

    template<template<typename> typename T, typename U, typename V, std::ptrdiff_t E>
        requires(concepts::unit_div_enabled<T<U>>)
    [[nodiscard]] friend constexpr auto operator/(const V& lhs,
                                                  const unit<T, U, E>& rhs) noexcept(noexcept(lhs / rhs.value))
    {
        return unit<T, std::common_type_t<U, V>, -E>{ lhs / rhs.value };
    }

    // comparison operators

    [[nodiscard]] friend constexpr auto operator<=>(const unit& lhs, const unit& rhs)
        requires(concepts::unit_cmp_enabled<UnitT<ValueT>>)
    = default;
};

} // namespace flow
