#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

#include "random.hpp"

namespace flow {
namespace detail {

    template<std::size_t N, typename T>
    using gradient_type = glm::vec<2, T, glm::packed_highp>;

    template<std::size_t N, typename T>
    struct lattice_point
    {
        using gradient_type = gradient_type<N, T>;
        using point_type = glm::vec<N, int, glm::packed_highp>;

        constexpr lattice_point(int x, int y) noexcept
            requires(N == 2)
            : sv{ x, y }
            , d{}
        {
            T ssv = static_cast<T>(sv.x + sv.y) * static_cast<T>(-0.211324865405187);
            d = -(gradient_type(sv) + ssv);
        }

        constexpr lattice_point(point_type point) noexcept
            requires(N == 2)
            : lattice_point(point.x, point.y)
        {}

        point_type sv;
        gradient_type d;
    };

    template<std::size_t N, typename T>
    inline constexpr T GradParam = static_cast<T>(1);

    template<typename T>
    inline constexpr T GradParam<2, T> = static_cast<T>(0.01001634121365712);

    template<std::size_t N, typename T>
    inline constexpr std::array<gradient_type<N, T>, 1> gradient_samples{};

    template<typename T>
    inline constexpr std::array gradient_samples<2, T> = {
        // clang-format off

        gradient_type<2, T>{ static_cast<T>( 0.130526192220052),  static_cast<T>( 0.99144486137381 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.38268343236509 ),  static_cast<T>( 0.923879532511287) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.608761429008721),  static_cast<T>( 0.793353340291235) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.793353340291235),  static_cast<T>( 0.608761429008721) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.923879532511287),  static_cast<T>( 0.38268343236509 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.99144486137381 ),  static_cast<T>( 0.130526192220051) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.99144486137381 ),  static_cast<T>(-0.130526192220051) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.923879532511287),  static_cast<T>(-0.38268343236509 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.793353340291235),  static_cast<T>(-0.60876142900872 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.608761429008721),  static_cast<T>(-0.793353340291235) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.38268343236509 ),  static_cast<T>(-0.923879532511287) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>( 0.130526192220052),  static_cast<T>(-0.99144486137381 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.130526192220052),  static_cast<T>(-0.99144486137381 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.38268343236509 ),  static_cast<T>(-0.923879532511287) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.608761429008721),  static_cast<T>(-0.793353340291235) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.793353340291235),  static_cast<T>(-0.608761429008721) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.923879532511287),  static_cast<T>(-0.38268343236509 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.99144486137381 ),  static_cast<T>(-0.130526192220052) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.99144486137381 ),  static_cast<T>( 0.130526192220051) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.923879532511287),  static_cast<T>( 0.38268343236509 ) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.793353340291235),  static_cast<T>( 0.608761429008721) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.608761429008721),  static_cast<T>( 0.793353340291235) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.38268343236509 ),  static_cast<T>( 0.923879532511287) } / GradParam<2, T>,
        gradient_type<2, T>{ static_cast<T>(-0.130526192220052),  static_cast<T>( 0.99144486137381 ) } / GradParam<2, T>

        // clang-format on
    };

    template<std::size_t N, typename T>
    inline constexpr std::array<lattice_point<N, T>, 1> lattice_points{};

    template<typename T>
    inline constexpr std::array lattice_points<2, T> = {
        // clang-format off

        lattice_point<2, T>{ 1, 0 },
        lattice_point<2, T>{ 0, 0 },
        lattice_point<2, T>{ 1, 1 },
        lattice_point<2, T>{ 0, 1 }

        // clang-format on
    };

    template<std::size_t N, typename T, std::size_t PermN>
    [[nodiscard]] consteval std::array<gradient_type<N, T>, PermN> generate_gradients() noexcept
    {
        std::array<gradient_type<N, T>, PermN> gradients{};

        for (std::size_t i = 0; i < gradients.size(); ++i)
        {
            gradients[i] = gradient_samples<N, T>[i % gradient_samples<N, T>.size()];
        }

        return gradients;
    }

} // namespace detail

template<std::size_t N, typename T = double, std::size_t PermN = 2048>
    requires(N >= 1 && N <= 3)
class open_simplex
{
private:
    static constexpr std::array gradients = detail::generate_gradients<N, T, PermN>();
    static constexpr glm::uint mask = static_cast<glm::uint>(PermN - 1);

public:
    template<typename U>
    using point_type = glm::vec<N, U, glm::qualifier::packed_highp>;
    using gradient_type = detail::gradient_type<N, T>;
    using permutation_type = std::int16_t;

public:
    constexpr open_simplex() noexcept = default;

    constexpr open_simplex(std::uint64_t seed)
    {
        m_permutations.resize(PermN);
        m_gradient_permutations.resize(PermN);

        std::iota(m_permutations.begin(), m_permutations.end(), 0);
        std::shuffle(m_permutations.begin(), m_permutations.end(), random_generator(seed));

        for (std::size_t i = 0; i < m_gradient_permutations.size(); ++i)
        {
            m_gradient_permutations[i] = gradients[m_permutations[i]];
        }
    }

    template<typename U = T>
    [[nodiscard]] constexpr U operator()(point_type<U> point) const noexcept
        requires(N == 2)
    {
        point += (point.x + point.y) * static_cast<U>(0.366025403784439);

        return base(point);
    }

    template<typename U = T>
    [[nodiscard]] constexpr U operator()(U x, U y) const noexcept
        requires(N == 2)
    {
        return operator()({ x, y });
    }

    template<typename U = T>
    [[nodiscard]] constexpr U base(point_type<U> point) const noexcept
        requires(N == 2)
    {
        U value{};

        point_type<U> point_base = glm::floor(point);
        point_type<U> point_offset = point - point_base;

        std::size_t index = static_cast<std::size_t>((point_offset.y - point_offset.x) / 2.0 + 1.0);

        point_offset += (point_offset.x + point_offset.y) * static_cast<U>(-0.211324865405187);

        for (std::size_t i = 0; i < 3; ++i)
        {
            auto lattice_point = detail::lattice_points<N, U>[index + i];
            point_type<U> d = point_offset + lattice_point.d;

            U attenuation = static_cast<U>(0.5) - glm::dot(d, d);

            if (attenuation <= static_cast<U>(0))
            {
                continue;
            }

            glm::uvec2 point_mask = glm::uvec2(static_cast<glm::uint>(point_base.x + lattice_point.sv.x) & mask,
                                               static_cast<glm::uint>(point_base.y + lattice_point.sv.y) & mask);
            point_type<U> gradient = m_gradient_permutations[m_permutations[point_mask.x] ^ point_mask.y];

            U extrapolation = glm::dot(gradient, d);

            attenuation *= attenuation;
            value += attenuation * attenuation * extrapolation;
        }

        return value;
    }

private:
    std::vector<permutation_type> m_permutations;
    std::vector<gradient_type> m_gradient_permutations;
};

template<typename GeneratorT>
class basic_noise_generator
{
public:
    using generator_type = GeneratorT;

public:
    constexpr basic_noise_generator(std::uint64_t seed)
        : m_generator(seed)
    {}

    template<typename... ArgsT>
    [[nodiscard]] constexpr decltype(auto) operator()(ArgsT&&... args) const noexcept(noexcept(m_generator(std::forward<ArgsT>(args)...)))
    {
        return m_generator(std::forward<ArgsT>(args)...);
    }

private:
    generator_type m_generator;
};

template<std::size_t N>
using noise_generator = basic_noise_generator<open_simplex<N, double, 2048>>;

} // namespace flow
