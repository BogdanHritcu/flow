#pragma once

#include <concepts>

#include "animation_controller.hpp"

namespace flow {

class animation
{
public:
    constexpr animation() = default;

    constexpr animation(flow::duration duration, bool reversed) noexcept
        : m_controller{ duration }
        , m_is_reversed{ reversed }
    {}

    constexpr virtual ~animation() = default;

    constexpr void advance(flow::duration dt) noexcept(noexcept(m_controller.advance(dt)))
    {
        static_assert(std::signed_integral<flow::duration::rep>);

        auto sign = 1 - 2 * static_cast<int>(m_is_reversed);
        m_controller.advance(sign * dt);

        update();
    }

    [[nodiscard]] constexpr bool is_reversed() const noexcept
    {
        return m_is_reversed;
    }

    [[nodiscard]] constexpr const flow::duration& duration() const noexcept
    {
        return m_controller.duration();
    }

    [[nodiscard]] constexpr flow::duration progress() const noexcept(noexcept(m_controller.progress()))
    {
        return m_controller.progress();
    }

    template<std::floating_point T = float>
    [[nodiscard]] constexpr T normalized_progress() const noexcept(noexcept(m_controller.normalized_progress<T>()))
    {
        return m_controller.normalized_progress<T>();
    }

    [[nodiscard]] constexpr bool has_finished() const noexcept
    {
        return (!m_is_reversed && m_controller.is_at_end())
                || (m_is_reversed && m_controller.is_at_start());
    }

protected:
    constexpr virtual void update() = 0;

private:
    flow::animation_controller m_controller{};
    bool m_is_reversed = false;
};

} // namespace flow
