#pragma once

#include <chrono>

namespace flow {

namespace concepts {
    template<typename T>
    concept clock = std::chrono::is_clock_v<T>;
}

template<concepts::clock T>
class clock_wrapper
{
public:
    using underlying_clock_type = T;
    using rep = typename underlying_clock_type::rep;
    using period = typename underlying_clock_type::period;
    using duration = typename underlying_clock_type::duration;
    using time_point = typename underlying_clock_type::time_point;
    static constexpr auto is_steady = underlying_clock_type::is_steady;

public:
    static time_point now() noexcept
    {
        return underlying_clock_type::now();
    }
};

using clock = clock_wrapper<std::chrono::high_resolution_clock>;
using duration = clock::duration;
using time_point = clock::time_point;

namespace detail {

    template<typename NewRep, typename NewPeriod, typename Rep, typename Period>
    [[nodiscard]] constexpr auto as_ratio_duration(std::chrono::duration<Rep, Period> duration) noexcept
    {
        return std::chrono::duration_cast<std::chrono::duration<NewRep, NewPeriod>>(duration);
    }

} // namespace detail

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_nanoseconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::nanoseconds::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_nanoseconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::nanoseconds(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_nanoseconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_nanoseconds_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_nanoseconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_nanoseconds_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_microseconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::microseconds::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_microseconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::microseconds(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_microseconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_microseconds_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_microseconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_microseconds_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_milliseconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::milliseconds::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_milliseconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::milliseconds(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_milliseconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_milliseconds_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_milliseconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_milliseconds_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_seconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::seconds::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_seconds_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::seconds(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_seconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_seconds_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_seconds(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_seconds_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_minutes_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::minutes::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_minutes_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::minutes(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_minutes(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_minutes_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_minutes(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_minutes_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_hours_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::hours::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_hours_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::hours(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_hours(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_hours_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_hours(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_hours_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_days_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::days::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_days_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::days(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_days(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_days_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_days(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_days_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_weeks_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::weeks::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_weeks_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::weeks(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_weeks(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_weeks_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_weeks(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_weeks_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_months_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::months::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_months_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::months(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_months(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_months_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_months(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_months_duration(duration).count();
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_years_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return detail::as_ratio_duration<NewRep, std::chrono::years::period>(duration);
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_years_duration(std::chrono::duration<Rep, Period> duration) noexcept
{
    return std::chrono::years(duration);
}

template<typename NewRep, typename Rep, typename Period>
[[nodiscard]] constexpr auto as_years(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_years_duration<NewRep>(duration).count();
}

template<typename Rep, typename Period>
[[nodiscard]] constexpr auto as_years(std::chrono::duration<Rep, Period> duration) noexcept
{
    return as_years_duration(duration).count();
}

} // namespace flow
