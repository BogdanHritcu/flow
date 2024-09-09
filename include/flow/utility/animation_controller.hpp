#pragma once

#include <algorithm>
#include <concepts>

#include "time.hpp"

namespace flow {

class animation_controller
{
private:
    static constexpr auto start_time_point = flow::time_point{};

public:
    constexpr animation_controller() noexcept = default;

    constexpr animation_controller(flow::duration duration) noexcept
        : m_duration{ duration }
    {}

    constexpr animation_controller(flow::duration duration, bool reversed, bool loop) noexcept
        : m_duration{ duration }
        , m_is_reversed{ reversed }
        , m_is_loop{ loop }
    {}

    constexpr void advance(flow::duration dt) noexcept(noexcept(m_t + dt - start_time_point))
    {
        static_assert(std::signed_integral<flow::duration::rep>);

        auto sign = 1 - 2 * static_cast<int>(m_is_reversed);
        auto duration_progress = m_t + sign * dt - start_time_point;

        if (duration_progress < flow::duration{ 0 })
        {
            duration_progress = m_is_loop ? m_duration + duration_progress % m_duration : flow::duration{ 0 };
        }
        else if (duration_progress > m_duration)
        {
            duration_progress = m_is_loop ? duration_progress % m_duration : m_duration;
        }

        m_t = start_time_point + duration_progress;
    }

    constexpr void forward(flow::duration dt) noexcept(noexcept(advance(dt)))
    {
        advance(dt);
    }

    constexpr void backward(flow::duration dt) noexcept(noexcept(advance(-dt)))
    {
        static_assert(std::signed_integral<flow::duration::rep>);

        advance(-dt);
    }

    constexpr void seek(flow::duration progress) noexcept(
            noexcept(std::clamp(flow::duration{},
                                flow::duration{},
                                flow::duration{})) && noexcept(start_time_point + flow::duration{}))
    {
        auto duration = std::clamp(progress, flow::duration{ 0 }, m_duration);
        m_t = start_time_point + duration;
    }

    constexpr void set_duration(flow::duration duration) noexcept
    {
        m_duration = duration;
    }

    constexpr void set_reversed(bool value) noexcept
    {
        m_is_reversed = value;
    }

    constexpr void set_loop(bool value) noexcept
    {
        m_is_loop = value;
    }

    constexpr void restart() noexcept
    {
        m_t = m_is_reversed ? start_time_point + m_duration : start_time_point;
    }

    [[nodiscard]] constexpr const flow::duration& duration() const noexcept
    {
        return m_duration;
    }

    [[nodiscard]] constexpr bool is_reversed() const noexcept
    {
        return m_is_reversed;
    }

    [[nodiscard]] constexpr bool is_loop() const noexcept
    {
        return m_is_loop;
    }

    [[nodiscard]] constexpr bool has_finished() const noexcept(noexcept(m_t == start_time_point + m_duration))
    {
        return !m_is_loop && is_at_end_r();
    }

    [[nodiscard]] constexpr flow::duration progress() const noexcept(noexcept(m_t - start_time_point))
    {
        return m_t - start_time_point;
    }

    [[nodiscard]] constexpr flow::duration progress_r() const noexcept(noexcept(m_t - start_time_point))
    {
        return m_is_reversed ? m_duration - (m_t - start_time_point) : m_t - start_time_point;
    }

    template<std::floating_point T = float>
    [[nodiscard]] constexpr T normalized_progress() const noexcept(noexcept(progress()) && noexcept(m_duration.count()))
    {
        return static_cast<T>(progress().count()) / static_cast<T>(m_duration.count());
    }

    template<std::floating_point T = float>
    [[nodiscard]] constexpr T normalized_progress_r() const noexcept(noexcept(progress()) && noexcept(m_duration.count()))
    {
        return static_cast<T>(progress_r().count()) / static_cast<T>(m_duration.count());
    }

    [[nodiscard]] constexpr bool is_at_start() const noexcept(noexcept(m_t == start_time_point))
    {
        return m_t == start_time_point;
    }

    [[nodiscard]] constexpr bool is_at_end() const noexcept(noexcept(m_t - start_time_point == m_duration))
    {
        return m_t - start_time_point == m_duration;
    }

    [[nodiscard]] constexpr bool is_at_start_r() const noexcept(noexcept(m_t == start_time_point))
    {
        return (!m_is_reversed && is_at_start()) || (m_is_reversed && is_at_end());
    }

    [[nodiscard]] constexpr bool is_at_end_r() const noexcept(noexcept(m_t - start_time_point == m_duration))
    {
        return (m_is_reversed && is_at_start()) || (!m_is_reversed && is_at_end());
    }

private:
    flow::time_point m_t = start_time_point;
    flow::duration m_duration{};
    bool m_is_reversed = false;
    bool m_is_loop = false;
};

} // namespace flow
