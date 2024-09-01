#pragma once

#include <concepts>

#include "../math/vec2.hpp"

namespace flow {

template<std::floating_point T>
class basic_linear_curve
{
public:
    using value_type = T;
    using vec2_type = flow::basic_vec2<T>;

public:
    constexpr basic_linear_curve() noexcept = default;

    constexpr basic_linear_curve(const concepts::vector_least2<value_type> auto& b0,
                                 const concepts::vector_least2<value_type> auto& b1) noexcept
        : m_b0{ b0.x, b0.y }
        , m_b1{ b1.x, b1.y }
    {
    }

    constexpr void set_points(const concepts::vector_least2<value_type> auto& b0,
                              const concepts::vector_least2<value_type> auto& b1) noexcept
    {
        m_b0 = { b0.x, b0.y };
        m_b1 = { b1.x, b1.y };
    }

    template<concepts::vector_least2<value_type> VecT = vec2_type>
    [[nodiscard]] constexpr VecT evaluate(value_type t) const noexcept
    {
        return (value_type{ 1 } - t) * VecT{ m_b0.x, m_b0.y } + t * VecT{ m_b1.x, m_b1.y };
    }

    template<concepts::vector_least2<value_type> VecT = vec2_type>
    [[nodiscard]] constexpr decltype(auto) operator()(value_type t) const noexcept
    {
        return evaluate<VecT>(t);
    }

private:
    vec2_type m_b0{};
    vec2_type m_b1{};
};

template<std::floating_point T>
class basic_quadratic_bezier_curve
{
public:
    using value_type = T;
    using vec2_type = flow::basic_vec2<T>;

public:
    constexpr basic_quadratic_bezier_curve() noexcept = default;

    constexpr basic_quadratic_bezier_curve(const vec2_type b0, const vec2_type b1, const vec2_type b2) noexcept
        : m_b0{ b0 }
        , m_b1{ b1 }
        , m_b2{ b2 }
    {
    }

    constexpr void set_points(const vec2_type b0, const vec2_type b1, const vec2_type b2) noexcept
    {
        m_b0 = b0;
        m_b1 = b1;
        m_b2 = b2;
    }

    [[nodiscard]] constexpr vec2_type evaluate(value_type t) const noexcept
    {
        auto u = value_type{ 1 } - t;

        return u * u * m_b0
                + value_type{ 2 } * u * t * m_b1
                + t * t * m_b2;
    }

    [[nodiscard]] constexpr vec2_type operator()(value_type t) const noexcept
    {
        return evaluate(t);
    }

private:
    vec2_type m_b0{};
    vec2_type m_b1{};
    vec2_type m_b2{};
};

template<std::floating_point T>
class basic_cubic_bezier_curve
{
public:
    using value_type = T;
    using vec2_type = flow::basic_vec2<T>;

public:
    constexpr basic_cubic_bezier_curve() noexcept = default;

    constexpr basic_cubic_bezier_curve(const vec2_type b0, const vec2_type b1, const vec2_type b2, const vec2_type b3) noexcept
        : m_b0{ b0 }
        , m_b1{ b1 }
        , m_b2{ b2 }
        , m_b3{ b3 }
    {
    }

    constexpr void set_points(const vec2_type b0, const vec2_type b1, const vec2_type b2, const vec2_type b3) noexcept
    {
        m_b0 = b0;
        m_b1 = b1;
        m_b2 = b2;
        m_b3 = b3;
    }

    [[nodiscard]] constexpr vec2_type evaluate(value_type t) const noexcept
    {
        auto u = value_type{ 1 } - t;
        auto ut3 = value_type{ 3 } * u * t;

        return u * u * u * m_b0
                + ut3 * u * m_b1
                + ut3 * t * m_b2
                + t * t * t * m_b3;
    }

    [[nodiscard]] constexpr vec2_type operator()(value_type t) const noexcept
    {
        return evaluate(t);
    }

private:
    vec2_type m_b0{};
    vec2_type m_b1{};
    vec2_type m_b2{};
    vec2_type m_b3{};
};

using linear_curve = basic_linear_curve<float>;
using quadratic_bezier_curve = basic_quadratic_bezier_curve<float>;
using cubic_bezier_curve = basic_cubic_bezier_curve<float>;

} // namespace flow
