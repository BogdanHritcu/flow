#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include "concepts.hpp"

namespace flow {

// based on Sebastiano Vigna's implementation
// https://prng.di.unimi.it/splitmix64.c
class split_mix64
{
public:
    using state_type = std::uint64_t;
    using result_type = std::uint64_t;

public:
    explicit constexpr split_mix64(state_type state = 0) noexcept
        : m_state{ state }
    {}

    [[nodiscard]] constexpr result_type operator()() noexcept
    {
        result_type z = (m_state += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }

    [[nodiscard]] constexpr state_type state() const noexcept
    {
        return m_state;
    }

    [[nodiscard]] static constexpr result_type min() noexcept
    {
        return std::numeric_limits<result_type>::min();
    }

    [[nodiscard]] static constexpr result_type max() noexcept
    {
        return std::numeric_limits<result_type>::max();
    }

private:
    state_type m_state;
};

// based on David Blackman and Sebastiano Vigna's implementation
// https://prng.di.unimi.it/xoshiro256plusplus.c
class xoshiro256pp
{
public:
    using result_type = std::uint64_t;
    using state_type = std::array<std::uint64_t, 4>;

public:
    explicit constexpr xoshiro256pp(std::uint64_t seed = 0) noexcept
        : m_state{}
    {
        split_mix64 generator(seed);
        for (auto& s : m_state)
        {
            s = generator();
        }
    }

    explicit constexpr xoshiro256pp(state_type state) noexcept
        : m_state{ state }
    {}

    [[nodiscard]] constexpr result_type operator()() noexcept
    {
        const result_type result = std::rotl(m_state[0] + m_state[3], 23) + m_state[0];

        const result_type t = m_state[1] << 17;

        m_state[2] ^= m_state[0];
        m_state[3] ^= m_state[1];
        m_state[1] ^= m_state[2];
        m_state[0] ^= m_state[3];

        m_state[2] ^= t;

        m_state[3] = std::rotl(m_state[3], 45);

        return result;
    }

    constexpr void jump() noexcept
    {
        using jump_type = state_type::value_type;

        constexpr std::array<jump_type, 4> jump_state{
            0x180ec6d33cfd0aba,
            0xd5a61266f0c9392c,
            0xa9582618e03fc9aa,
            0x39abdc4529b1661c
        };

        constexpr std::size_t jump_bit_count = sizeof(jump_type) * 8;

        state_type state{};

        for (jump_type jump : jump_state)
        {
            for (std::size_t b = 0; b < jump_bit_count; ++b)
            {
                if (jump & static_cast<jump_type>(1) << b)
                {
                    for (std::size_t i = 0; i < m_state.size(); ++i)
                    {
                        state[i] ^= m_state[i];
                    }
                }
                std::ignore = operator()();
            }
        }

        m_state.swap(state);
    }

    constexpr void long_jump() noexcept
    {
        using jump_type = state_type::value_type;

        constexpr std::array<jump_type, 4> jump_state{
            0x76e15d3efefdcbbf,
            0xc5004e441c522fb3,
            0x77710069854ee241,
            0x39109bb02acbe635
        };

        constexpr std::size_t jump_bit_count = sizeof(jump_type) * 8;

        state_type state{};

        for (jump_type jump : jump_state)
        {
            for (std::size_t b = 0; b < jump_bit_count; ++b)
            {
                if (jump & static_cast<jump_type>(1) << b)
                {
                    for (std::size_t i = 0; i < m_state.size(); ++i)
                    {
                        state[i] ^= m_state[i];
                    }
                }
                std::ignore = operator()();
            }
        }

        m_state.swap(state);
    }

    [[nodiscard]] constexpr state_type state() const noexcept
    {
        return m_state;
    }

    [[nodiscard]] static constexpr result_type min() noexcept
    {
        return std::numeric_limits<result_type>::min();
    }

    [[nodiscard]] static constexpr result_type max() noexcept
    {
        return std::numeric_limits<result_type>::max();
    }

private:
    state_type m_state;
};

// TODO: find a way to make constexpr the methods of the
// basic_random_generator that generate uniform distributions

template<typename GeneratorT>
class basic_random_generator
{
public:
    using generator_type = GeneratorT;

    using result_type = typename generator_type::result_type;

public:
    explicit constexpr basic_random_generator(std::uint64_t seed = 0) noexcept(concepts::nothrow_constructible<generator_type, std::uint64_t>)
        : m_generator(seed)
    {}

    template<concepts::arithmetic T = result_type>
    [[nodiscard]] constexpr T operator()() noexcept(concepts::nothrow_operator_callable<generator_type>)
    {
        return next<T>();
    }

    template<concepts::arithmetic T = result_type>
    [[nodiscard]] constexpr T next() noexcept(concepts::nothrow_operator_callable<generator_type>)
    {
        static_assert(sizeof(result_type) >= sizeof(T), "result type of generator is too small");

        if constexpr (std::same_as<T, result_type>)
        {
            return m_generator();
        }
        else if constexpr (concepts::single_precision_float<T>)
        {
            return static_cast<T>((next<std::uint32_t>() >> 8) * 0x1.0p-24f);
        }
        else if constexpr (concepts::double_precision_float<T>)
        {
            return static_cast<T>((next<std::uint64_t>() >> 11) * 0x1.0p-53);
        }
        else if constexpr (concepts::boolean<T>)
        {
            return static_cast<T>(next<result_type>() >> (sizeof(result_type) * 8 - 1));
        }
        else
        {
            return static_cast<T>(next<result_type>() >> ((sizeof(result_type) - sizeof(T)) * 8));
        }
    }

    template<concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] T uniform(T min, T max)
    {
        if constexpr (concepts::single_precision_float<T> || concepts::double_precision_float<T>)
        {
            return std::uniform_real_distribution<T>(min, max)(*this);
        }
        else
        {
            return std::uniform_int_distribution<T>(min, max)(*this);
        }
    }

    template<concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] T uniform(T max)
    {
        return uniform<T>(static_cast<T>(0), max);
    }

    template<concepts::arithmetic T = result_type>
    [[nodiscard]] T uniform()
    {
        if constexpr (concepts::single_precision_float<T> || concepts::double_precision_float<T>)
        {
            return uniform<T>(static_cast<T>(0), static_cast<T>(1));
            // TODO: check if next method returns uniform numbers for floats [0.0, 1.0]
        }
        else if constexpr (concepts::boolean<T>)
        {
            return bernoulli(0.5);
        }
        else
        {
            return uniform<T>(min<T>(), max<T>());
        }
    }

    [[nodiscard]] bool bernoulli(double p)
    {
        return std::bernoulli_distribution(p)(*this);
    }

    template<concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] constexpr std::vector<T> sequence(std::size_t n)
    {
        std::vector<T> numbers(n);

        for (auto& number : numbers)
        {
            number = next<T>();
        }

        return numbers;
    }

    template<std::size_t N, concepts::arithmetic T = result_type>
    [[nodiscard]] constexpr std::array<T, N> sequence() noexcept(concepts::nothrow_operator_callable<generator_type>)
    {
        std::array<T, N> numbers{};

        for (auto& number : numbers)
        {
            number = next<T>();
        }

        return numbers;
    }

    template<concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] std::vector<T> uniform_sequence(std::size_t n, T min, T max)
    {
        std::vector<T> numbers(n);

        for (auto& number : numbers)
        {
            number = uniform<T>(min, max);
        }

        return numbers;
    }

    template<concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] std::vector<T> uniform_sequence(std::size_t n, T max)
    {
        return uniform_sequence<T>(n, static_cast<T>(0), max);
    }

    template<concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] std::vector<T> uniform_sequence(std::size_t n)
    {
        if constexpr (concepts::single_precision_float<T> || concepts::double_precision_float<T>)
        {
            return uniform_sequence<T>(n, static_cast<T>(0), static_cast<T>(1));
        }
        else
        {
            return uniform_sequence<T>(n, min<T>(), max<T>());
        }
    }

    template<std::size_t N, concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] std::array<T, N> uniform_sequence(T min, T max)
    {
        std::array<T, N> numbers{};

        for (auto& number : numbers)
        {
            number = uniform<T>(min, max);
        }

        return numbers;
    }

    template<std::size_t N, concepts::non_boolean_arithmetic T = result_type>
    [[nodiscard]] std::array<T, N> uniform_sequence(T max)
    {
        return uniform_sequence<N, T>(static_cast<T>(0), max);
    }

    template<std::size_t N, concepts::arithmetic T = result_type>
    [[nodiscard]] std::array<T, N> uniform_sequence()
    {
        if constexpr (concepts::single_precision_float<T> || concepts::double_precision_float<T>)
        {
            return uniform_sequence<N, T>(static_cast<T>(0), static_cast<T>(1));
        }
        else if constexpr (concepts::boolean<T>)
        {
            return benoulli_sequence<N>(0.5);
        }
        else
        {
            return uniform_sequence<N, T>(min<T>(), max<T>());
        }
    }

    template<std::size_t N>
    [[nodiscard]] std::array<bool, N> benoulli_sequence(double p)
    {
        std::array<bool, N> numbers{};

        for (auto& number : numbers)
        {
            number = bernoulli(p);
        }

        return numbers;
    }

    template<concepts::arithmetic T = result_type>
    [[nodiscard]] static constexpr T min() noexcept
    {
        return std::numeric_limits<T>::min();
    }

    template<concepts::arithmetic T = result_type>
    [[nodiscard]] static constexpr T max() noexcept
    {
        return std::numeric_limits<T>::max();
    }

private:
    generator_type m_generator;
};

using random_generator = basic_random_generator<xoshiro256pp>;

} // namespace flow
