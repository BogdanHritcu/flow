#pragma once

#include <chrono>

namespace flow {

template<typename ClockT>
class basic_duration
{
public:
    using clock = ClockT;
    using rep = typename clock::rep;
    using period = typename clock::period;
    using underlying_duration = typename clock::duration;

    constexpr basic_duration()
        : m_duration{}
    {}

    constexpr basic_duration(const underlying_duration& duration)
        : m_duration{ std::chrono::duration_cast<decltype(m_duration)>(duration) }
    {}

    template<typename Rep>
    constexpr explicit basic_duration(const Rep& ticks)
        : m_duration{ ticks }
    {}

    template<typename Period, typename Rep = double>
    [[nodiscard]] constexpr Rep to() const
    {
        return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(m_duration).count();
    }

    template<typename Rep = double>
    [[nodiscard]] constexpr Rep seconds() const
    {
        return to<std::ratio<1, 1>, Rep>();
    }

    template<typename Rep = double>
    [[nodiscard]] constexpr Rep milliseconds() const
    {
        return to<std::milli, Rep>();
    }

    template<typename Rep = double>
    [[nodiscard]] constexpr Rep microseconds() const
    {
        return to<std::micro, Rep>();
    }

private:
    underlying_duration m_duration;
};

template<typename ClockT>
class basic_time_point
{
public:
    using clock = ClockT;
    using duration = typename clock::duration;
    using underlying_time_point = typename clock::time_point;

    constexpr basic_time_point() = default;
    constexpr explicit basic_time_point(const basic_time_point& other) = default;

    constexpr explicit basic_time_point(const underlying_time_point& time_point)
        : m_time_point{ time_point }
    {}

    constexpr explicit basic_time_point(const duration& duration)
        : m_time_point{ duration }
    {}

private:
    underlying_time_point m_time_point;

    // operators
    template<typename T>
    friend constexpr basic_duration<T> operator-(const basic_time_point<T>& lhs,
                                                 const basic_time_point<T>& rhs)
    {
        return basic_duration<T>{ lhs.m_time_point - rhs.m_time_point };
    }
};

template<typename UnderlyingClockT>
class basic_clock
{
public:
    using underlying_clock = UnderlyingClockT;
    using rep = typename underlying_clock::rep;
    using period = typename underlying_clock::period;
    using duration = typename underlying_clock::duration;
    using time_point = typename underlying_clock::time_point;
    using clk_time_point = basic_time_point<basic_clock>;

    static clk_time_point now() noexcept
    {
        return clk_time_point{ underlying_clock::now() };
    }
};

template<typename ClockT>
class basic_stopwatch
{
public:
    using clock = ClockT;
    using duration = basic_duration<clock>;
    using time_point = basic_time_point<clock>;

    basic_stopwatch()
        : m_start_point{ clock::now() }
    {}

    duration elapsed() const noexcept
    {
        return clock::now() - m_start_point;
    }

    duration reset() noexcept
    {
        time_point old_start_point{ m_start_point };
        m_start_point = clock::now();

        return m_start_point - old_start_point;
    }

private:
    time_point m_start_point;
};

using clock = basic_clock<std::chrono::high_resolution_clock>;
using duration = basic_duration<clock>;
using time_point = basic_time_point<clock>;
using stopwatch = basic_stopwatch<clock>;

} // namespace flow
