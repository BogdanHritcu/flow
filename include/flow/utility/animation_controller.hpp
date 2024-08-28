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

    constexpr void advance(flow::duration dt) noexcept(
            noexcept(m_t + flow::duration{}) && noexcept(m_t.time_since_epoch()) && noexcept(seek(flow::duration{})))
    {
        seek((m_t + dt).time_since_epoch());
    }

    constexpr void forward(flow::duration dt) noexcept(
            noexcept(m_t + flow::duration{}) && noexcept(m_t.time_since_epoch()) && noexcept(seek(flow::duration{})))
    {
        advance(dt);
    }

    constexpr void backward(flow::duration dt) noexcept(
            noexcept(m_t - flow::duration{}) && noexcept(m_t.time_since_epoch()) && noexcept(seek(flow::duration{})))
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

    constexpr void reset() noexcept
    {
        m_t = start_time_point;
    }

    [[nodiscard]] constexpr const flow::duration& duration() const noexcept
    {
        return m_duration;
    }

    [[nodiscard]] constexpr flow::duration progress() const noexcept(noexcept(m_t - start_time_point))
    {
        return m_t - start_time_point;
    }

    template<std::floating_point T = float>
    [[nodiscard]] constexpr T normalized_progress() const noexcept(noexcept(progress()) && noexcept(m_duration.count()))
    {
        return static_cast<T>(progress().count()) / static_cast<T>(m_duration.count());
    }

    [[nodiscard]] constexpr bool is_at_start() const noexcept(noexcept(m_t == start_time_point))
    {
        return m_t == start_time_point;
    }

    [[nodiscard]] constexpr bool is_at_end() const noexcept(
            noexcept(m_t.time_since_epoch()) && noexcept(m_t.time_since_epoch() == m_duration))
    {
        return m_t.time_since_epoch() == m_duration;
    }

private:
    flow::time_point m_t = start_time_point;
    flow::duration m_duration{};
};

} // namespace flow
