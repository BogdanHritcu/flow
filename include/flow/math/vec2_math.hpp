#pragma once

#include <algorithm>
#include <cmath>

namespace flow {

template<typename Vec2T>
[[nodiscard]] constexpr decltype(auto) dot(const Vec2T& a, const Vec2T& b) noexcept(noexcept(a.x * b.x + a.y * b.y))
{
    return a.x * b.x + a.y * b.y;
}

template<typename Vec2T>
[[nodiscard]] constexpr decltype(auto) cross(const Vec2T& a, const Vec2T& b) noexcept(noexcept(a.x * b.y - a.y * b.x))
{
    return a.x * b.y - a.y * b.x;
}

template<typename Vec2T>
[[nodiscard]] constexpr decltype(auto) length2(const Vec2T& v) noexcept(noexcept(dot(v, v)))
{
    return dot(v, v);
}

template<typename Vec2T>
[[nodiscard]] decltype(auto) length(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::sqrt(length2(v)))))
{
    return static_cast<decltype(v.x)>(std::sqrt(length2(v)));
}

template<typename Vec2T>
[[nodiscard]] Vec2T normalize(Vec2T v) noexcept(noexcept(v /= length(v)))
{
    return v /= length(v);
}

template<typename Vec2T>
[[nodiscard]] constexpr decltype(auto) distance2(Vec2T a, const Vec2T& b) noexcept(noexcept(length2(a -= b)))
{
    return length2(a -= b);
}

template<typename Vec2T>
[[nodiscard]] decltype(auto) distance(Vec2T a, const Vec2T& b) noexcept(noexcept(length(a -= b)))
{
    return length(a -= b);
}

template<typename Vec2T>
[[nodiscard]] constexpr Vec2T reflect(const Vec2T& v, const Vec2T& n) noexcept(noexcept(v - n * (dot(v, n) * decltype(v.x){ 2 })))
{
    return v - n * (dot(v, n) * decltype(v.x){ 2 });
}

template<typename Vec2T>
[[nodiscard]] constexpr Vec2T min(const Vec2T& a, const Vec2T& b) noexcept(noexcept(std::min(a.x, b.x)))
{
    return { std::min(a.x, b.x), std::min(a.y, b.y) };
}

template<typename Vec2T>
[[nodiscard]] constexpr Vec2T max(const Vec2T& a, const Vec2T& b) noexcept(noexcept(std::max(a.x, b.x)))
{
    return { std::max(a.x, b.x), std::max(a.y, b.y) };
}

template<typename Vec2T>
[[nodiscard]] constexpr Vec2T clamp(const Vec2T& v,
                                    const Vec2T& min,
                                    const Vec2T& max) noexcept(noexcept(std::clamp(v.x, min.x, max.x)))
{
    return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T abs(const Vec2T& v) noexcept(noexcept(std::abs(v.x)))
{
    return { std::abs(v.x), std::abs(v.y) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T sqrt(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::sqrt(v.x))))
{
    return { static_cast<decltype(v.x)>(std::sqrt(v.x)), static_cast<decltype(v.x)>(std::sqrt(v.y)) };
}

template<typename Vec2T, typename U>
[[nodiscard]] Vec2T pow(const Vec2T& v, U exp) noexcept(noexcept(static_cast<decltype(v.x)>(std::pow(v.x, exp))))
{
    return { static_cast<decltype(v.x)>(std::pow(v.x, exp)), static_cast<decltype(v.x)>(std::pow(v.y, exp)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T exp(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::exp(v.x))))
{
    return { static_cast<decltype(v.x)>(std::exp(v.x)), static_cast<decltype(v.x)>(std::exp(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T log(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::log(v.x))))
{
    return { static_cast<decltype(v.x)>(std::log(v.x)), static_cast<decltype(v.x)>(std::log(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T exp2(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::exp2(v.x))))
{
    return { static_cast<decltype(v.x)>(std::exp2(v.x)), static_cast<decltype(v.x)>(std::exp2(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T log2(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::log2(v.x))))
{
    return { static_cast<decltype(v.x)>(std::log2(v.x)), static_cast<decltype(v.x)>(std::log2(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T sin(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::sin(v.x))))
{
    return { static_cast<decltype(v.x)>(std::sin(v.x)), static_cast<decltype(v.x)>(std::sin(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T cos(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::cos(v.x))))
{
    return { static_cast<decltype(v.x)>(std::cos(v.x)), static_cast<decltype(v.x)>(std::cos(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T tan(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::tan(v.x))))
{
    return { static_cast<decltype(v.x)>(std::tan(v.x)), static_cast<decltype(v.x)>(std::tan(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T asin(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::asin(v.x))))
{
    return { static_cast<decltype(v.x)>(std::asin(v.x)), static_cast<decltype(v.x)>(std::asin(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T acos(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::acos(v.x))))
{
    return { static_cast<decltype(v.x)>(std::acos(v.x)), static_cast<decltype(v.x)>(std::acos(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] Vec2T atan(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::atan(v.x))))
{
    return { static_cast<decltype(v.x)>(std::atan(v.x)), static_cast<decltype(v.y)>(std::atan(v.y)) };
}

template<typename Vec2T>
[[nodiscard]] decltype(auto) atan2(const Vec2T& v) noexcept(noexcept(static_cast<decltype(v.x)>(std::atan2(v.y, v.x))))
{
    return static_cast<decltype(v.x)>(std::atan2(v.y, v.x));
}

} // namespace flow
