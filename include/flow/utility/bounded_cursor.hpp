#pragma once

#include <algorithm>
#include <concepts>

#include "integer_range.hpp"
#include "numeric.hpp"

namespace flow {

template<std::unsigned_integral T>
class bounded_cursor
{
public:
    using unit_type = T;
    using bounds_type = ie_integer_range<unit_type>;

public:
    constexpr bounded_cursor() noexcept = default;

    constexpr bounded_cursor(unit_type start, unit_type end, unit_type position) noexcept
        : m_bounds{ std::min(start, end), std::max(start, end) }
        , m_position{ std::clamp(position, start, end) }
    {}

    constexpr bounded_cursor(unit_type start, unit_type end) noexcept
        : bounded_cursor(start, end, start)
    {}

    constexpr void forward(unit_type units) noexcept
    {
        m_position = std::min(m_bounds.end, add_sat(m_position, units));
    }

    constexpr void backward(unit_type units) noexcept
    {
        m_position = std::max(m_bounds.start, sub_sat(m_position, units));
    }

    constexpr void forward_inc(unit_type units) noexcept
    {
        m_position = add_sat(m_position, units);
        m_bounds.end = std::max(m_position, m_bounds.end);
    }

    constexpr void backward_dec(unit_type units) noexcept
    {
        m_position = sub_sat(m_position, units);
        m_bounds.start = std::min(m_position, m_bounds.start);
    }

    constexpr void seek(unit_type position) noexcept
    {
        m_position = std::clamp(position, m_bounds.start, m_bounds.end);
    }

    constexpr void seek_inc_dec(unit_type position) noexcept
    {
        m_position = position;
        m_bounds.start = std::min(m_position, m_bounds.start);
        m_bounds.end = std::max(m_position, m_bounds.end);
    }

    constexpr void inc_start(unit_type units) noexcept
    {
        m_bounds.start = std::min(add_sat(m_bounds.start, units), m_bounds.end);
        m_position = std::max(m_bounds.start, m_position);
    }

    constexpr void inc_end(unit_type units) noexcept
    {
        m_bounds.end = add_sat(m_bounds.end, units);
    }

    constexpr void dec_start(unit_type units) noexcept
    {
        m_bounds.start = sub_sat(m_bounds.start, units);
    }

    constexpr void dec_end(unit_type units) noexcept
    {
        m_bounds.end = std::max(sub_sat(m_bounds.end, units), m_bounds.start);
        m_position = std::min(m_bounds.end, m_position);
    }

    [[nodiscard]] constexpr const bounds_type& bounds() const noexcept
    {
        return m_bounds;
    }

    [[nodiscard]] constexpr unit_type position() const noexcept
    {
        return m_position;
    }

    [[nodiscard]] constexpr unit_type start() const noexcept
    {
        return m_bounds.start;
    }

    [[nodiscard]] constexpr unit_type end() const noexcept
    {
        return m_bounds.end;
    }

    [[nodiscard]] constexpr unit_type size() const noexcept
    {
        return m_bounds.end - m_bounds.end;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
        return m_bounds.contains(m_position);
    }

private:
    bounds_type m_bounds{};
    unit_type m_position{};
};

} // namespace flow
