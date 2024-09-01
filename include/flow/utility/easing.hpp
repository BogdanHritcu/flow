#pragma once

#include <cmath>
#include <concepts>
#include <numbers>

namespace flow {

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_quadratic(T t) noexcept
{
    return t * t;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_quadratic(T t) noexcept
{
    auto u = T{ 1 } - t;
    return T{ 1 } - u * u;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_out_quadratic(T t) noexcept
{
    return t < T{ 0.5 } ? T{ 2 } * t * t
                        : T{ 1 } - (T{ -2 } * t + T{ 2 }) * (T{ -2 } * t + T{ 2 }) * T{ 0.5 };
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_cubic(T t) noexcept
{
    return t * t * t;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_cubic(T t) noexcept
{
    auto u = T{ 1 } - t;
    return T{ 1 } - u * u * u;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_quartic(T t) noexcept
{
    return t * t * t * t;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_quartic(T t) noexcept
{
    auto u = T{ 1 } - t;
    return T{ 1 } - u * u * u * u;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_quintic(T t) noexcept
{
    return t * t * t * t * t;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_quintic(T t) noexcept
{
    auto u = T{ 1 } - t;
    return T{ 1 } - u * u * u * u * u;
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_elastic(T t) noexcept
{
    using namespace std::numbers;
    static constexpr auto pi2o3 = pi_v<T> * T{ 2 } / T{ 3 };

    return t == T{ 0 } || t == T{ 1 } ? t
                                      : -std::pow(T{ 2 }, T{ 10 } * t - T{ 10 }) * std::sin((t * T{ 10 } - T{ 10.75 }) * pi2o3);
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_elastic(T t) noexcept
{
    using namespace std::numbers;
    static constexpr auto pi2o3 = pi_v<T> * T{ 2 } / T{ 3 };

    return t == T{ 0 } || t == T{ 1 } ? t
                                      : std::pow(T{ 2 }, T{ -10 } * t) * std::sin((t * T{ 10 } - T{ 0.75 }) * pi2o3) + T{ 1 };
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_bounce(T t) noexcept
{
    static constexpr auto n1 = T{ 7.5625 };
    static constexpr auto d1 = T{ 2.75 };

    if (t < T{ 1 } / d1)
    {
        return n1 * t * t;
    }
    else if (t < T{ 2 } / d1)
    {
        return n1 * (t -= T{ 1.5 } / d1) * t + T{ 0.75 };
    }
    else if (t < T{ 2.5 } / d1)
    {
        return n1 * (t -= T{ 2.25 } / d1) * t + T{ 0.9375 };
    }

    return n1 * (t -= T{ 2.625 } / d1) * t + T{ 0.984375 };
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_bounce(T t) noexcept
{
    return T{ 1 } - ease_out_bounce(T{ 1 } - t);
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_exponential(T t) noexcept
{
    return T{ 1 } - static_cast<T>(t != T{ 1 }) * std::pow(T{ 2 }, T{ -10 } * t);
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_exponential(T t) noexcept
{
    return static_cast<T>(t != T{ 0 }) * std::pow(T{ 2 }, T{ 10 } * t - T{ 10 });
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_out_back(T t) noexcept
{
    static constexpr auto c = T{ 1.70158 };
    auto u = t - T{ 1 };

    return T{ 1 } + u * u * (c * (u + 1) + u);
}

template<std::floating_point T = float>
[[nodiscard]] constexpr T ease_in_back(T t) noexcept
{
    static constexpr auto c = T{ 1.70158 };

    return t * t * (c * (t - T{ 1 }) + t);
}

} // namespace flow
