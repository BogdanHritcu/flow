#pragma once

#include "time.hpp"

namespace flow {

template<concepts::clock ClockT>
class basic_stopwatch
{
public:
    using clock_type = ClockT;
    using duration = typename clock_type::duration;
    using time_point = typename clock_type::time_point;

public:
    basic_stopwatch() noexcept
        : m_start_point(clock_type::now())
    {}

    [[nodiscard]] duration elapsed() const noexcept
    {
        return clock_type::now() - m_start_point;
    }

    time_point reset() noexcept
    {
        m_start_point = clock_type::now();

        return m_start_point;
    }

private:
    time_point m_start_point{};
};

using stopwatch = basic_stopwatch<clock>;

} // namespace flow
