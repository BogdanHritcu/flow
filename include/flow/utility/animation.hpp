#pragma once

#include <concepts>

#include "animation_controller.hpp"

namespace flow {

class animation
{
public:
    constexpr animation() = default;

    constexpr animation(flow::duration duration) noexcept
        : m_controller(duration)
    {}

    constexpr animation(flow::duration duration, bool reversed, bool loop) noexcept
        : m_controller(duration, reversed, loop)
    {}

    constexpr virtual ~animation() = default;

    constexpr void advance(flow::duration dt) noexcept(noexcept(m_controller.advance(dt)))
    {
        m_controller.advance(dt);
        update();
    }

    constexpr void seek(flow::duration progress) noexcept(noexcept(m_controller.seek(progress)))
    {
        m_controller.seek(progress);
    }

    constexpr void set_duration(flow::duration duration) noexcept
    {
        m_controller.set_duration(duration);
    }

    constexpr void set_reversed(bool value) noexcept
    {
        m_controller.set_reversed(value);
    }

    constexpr void set_loop(bool value) noexcept
    {
        m_controller.set_loop(value);
    }

    constexpr virtual void restart() noexcept
    {
        m_controller.restart();
    }

    [[nodiscard]] constexpr bool is_reversed() const noexcept
    {
        return m_controller.is_reversed();
    }

    [[nodiscard]] constexpr bool is_loop() const noexcept
    {
        return m_controller.is_loop();
    }

    [[nodiscard]] constexpr const flow::duration& duration() const noexcept
    {
        return m_controller.duration();
    }

    [[nodiscard]] constexpr flow::duration progress() const noexcept(noexcept(m_controller.progress()))
    {
        return m_controller.progress();
    }

    [[nodiscard]] constexpr bool has_finished() const noexcept(noexcept(m_controller.has_finished()))
    {
        return m_controller.has_finished();
    }

    template<std::floating_point T = float>
    [[nodiscard]] constexpr T normalized_progress() const noexcept(noexcept(m_controller.normalized_progress<T>()))
    {
        return m_controller.normalized_progress<T>();
    }

protected:
    constexpr virtual void update() = 0;

private:
    flow::animation_controller m_controller{};
};

} // namespace flow
