#pragma once

#include <concepts>

#include "../math/vec2.hpp"

namespace flow {

template<std::floating_point T>
class basic_quadratic_bezier
{
public:
    using value_type = T;
    using vec2_type = flow::basic_vec2<T>;

public:
    constexpr basic_quadratic_bezier() noexcept = default;

    constexpr basic_quadratic_bezier(const vec2_type b0, const vec2_type b1, const vec2_type b2) noexcept
        : m_b0{ b0 }
        , m_b1{ b1 }
        , m_b2{ b2 }
    {
    }

    void set_points(const vec2_type b0, const vec2_type b1, const vec2_type b2) noexcept
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
class basic_cubic_bezier
{
public:
    using value_type = T;
    using vec2_type = flow::basic_vec2<T>;

public:
    constexpr basic_cubic_bezier() noexcept = default;

    constexpr basic_cubic_bezier(const vec2_type b0, const vec2_type b1, const vec2_type b2, const vec2_type b3) noexcept
        : m_b0{ b0 }
        , m_b1{ b1 }
        , m_b2{ b2 }
        , m_b3{ b3 }
    {
    }

    void set_points(const vec2_type b0, const vec2_type b1, const vec2_type b2, const vec2_type b3) noexcept
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

using quadratic_bezier = basic_quadratic_bezier<float>;
using cubic_bezier = basic_cubic_bezier<float>;

} // namespace flow
