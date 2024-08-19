#pragma once

namespace flow {

template<typename T>
struct basic_vec2
{
    using value_type = T;

    value_type x;
    value_type y;

    constexpr basic_vec2& operator+=(const basic_vec2& v) noexcept(noexcept(x += v.x))
    {
        x += v.x;
        y += v.y;

        return *this;
    }

    constexpr basic_vec2& operator+=(value_type s) noexcept(noexcept(x += s))
    {
        x += s;
        y += s;

        return *this;
    }

    constexpr basic_vec2& operator-=(const basic_vec2& v) noexcept(noexcept(x -= v.x))
    {
        x -= v.x;
        y -= v.y;

        return *this;
    }

    constexpr basic_vec2& operator-=(value_type s) noexcept(noexcept(x -= s))
    {
        x -= s;
        y -= s;

        return *this;
    }

    constexpr basic_vec2& operator*=(const basic_vec2& v) noexcept(noexcept(x *= v.x))
    {
        x *= v.x;
        y *= v.y;

        return *this;
    }

    constexpr basic_vec2& operator*=(value_type s) noexcept(noexcept(x *= s))
    {
        x *= s;
        y *= s;

        return *this;
    }

    constexpr basic_vec2& operator/=(const basic_vec2& v) noexcept(noexcept(x /= v.x))
    {
        x /= v.x;
        y /= v.y;

        return *this;
    }

    constexpr basic_vec2& operator/=(value_type s) noexcept(noexcept(x /= s))
    {
        x /= s;
        y /= s;

        return *this;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept(noexcept(static_cast<bool>(x)))
    {
        return static_cast<bool>(x) && static_cast<bool>(y);
    }

    template<typename Vec2U>
    [[nodiscard]] explicit constexpr operator Vec2U() const noexcept
    {
        return { x, y };
    }

    template<typename U>
    [[nodiscard]] explicit constexpr operator basic_vec2<U>() const noexcept
    {
        return { static_cast<U>(x), static_cast<U>(y) };
    }

    [[nodiscard]] friend constexpr basic_vec2 operator+(basic_vec2 a, const basic_vec2& b) noexcept(noexcept(a += b))
    {
        return a += b;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator+(basic_vec2 v, value_type s) noexcept(noexcept(v += s))
    {
        return v += s;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator+(value_type s, const basic_vec2& v) noexcept(noexcept(s + v.x))
    {
        return { s + v.x, s + v.y };
    }

    [[nodiscard]] friend constexpr basic_vec2 operator-(basic_vec2 a, const basic_vec2& b) noexcept(noexcept(a -= b))
    {
        return a -= b;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator-(basic_vec2 v, value_type s) noexcept(noexcept(v -= s))
    {
        return v -= s;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator*(basic_vec2 a, const basic_vec2& b) noexcept(noexcept(a *= b))
    {
        return a *= b;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator*(basic_vec2 v, value_type s) noexcept(noexcept(v *= s))
    {
        return v *= s;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator*(value_type s, const basic_vec2& v) noexcept(noexcept(s * v.x))
    {
        return { s * v.x, s * v.y };
    }

    [[nodiscard]] friend constexpr basic_vec2 operator/(basic_vec2 a, const basic_vec2& b) noexcept(noexcept(a /= b))
    {
        return a /= b;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator/(basic_vec2 v, value_type s) noexcept(noexcept(v /= s))
    {
        return v /= s;
    }

    [[nodiscard]] friend constexpr basic_vec2 operator-(const basic_vec2& v) noexcept(noexcept(-v.x))
    {
        return { -v.x, -v.y };
    }
};

using vec2 = basic_vec2<float>;

} // namespace flow
