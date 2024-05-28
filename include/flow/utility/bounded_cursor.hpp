#pragma once

#include <algorithm>
#include <concepts>

#include "helpers.hpp"
#include "integer_range.hpp"

namespace flow {

template<std::unsigned_integral T>
class bounded_cursor
{
public:
    using unit_type = T;
    using bounds_type = ii_integer_range<unit_type>;

public:
    constexpr bounded_cursor() noexcept = default;

    constexpr bounded_cursor(unit_type left, unit_type right, unit_type cursor) noexcept
        : m_bounds{ std::min(left, right), std::max(left, right) }
        , m_cursor{ std::clamp(cursor, left, right) }
    {}

    constexpr bounded_cursor(unit_type left, unit_type right) noexcept
        : bounded_cursor(left, right, left)
    {}

    constexpr void forward(unit_type units) noexcept
    {
        m_cursor = std::min(m_bounds.end, add_sat(m_cursor, units));
    }

    constexpr void backward(unit_type units) noexcept
    {
        m_cursor = std::max(m_bounds.start, sub_sat(m_cursor, units));
    }

    constexpr void forward_inc(unit_type units) noexcept
    {
        m_cursor = add_sat(m_cursor, units);
        m_bounds.end = std::max(m_cursor, m_bounds.end);
    }

    constexpr void backward_dec(unit_type units) noexcept
    {
        m_cursor = sub_sat(m_cursor, units);
        m_bounds.start = std::min(m_cursor, m_bounds.start);
    }

    constexpr void seek(unit_type position) noexcept
    {
        m_cursor = std::clamp(position, m_bounds.start, m_bounds.end);
    }

    constexpr void seek_inc_dec(unit_type position) noexcept
    {
        m_cursor = position;
        m_bounds.start = std::min(m_cursor, m_bounds.start);
        m_bounds.end = std::max(m_cursor, m_bounds.end);
    }

    constexpr void inc_left(unit_type units) noexcept
    {
        m_bounds.start = std::min(add_sat(m_bounds.start, units), m_bounds.end);
        m_cursor = std::max(m_bounds.start, m_cursor);
    }

    constexpr void inc_right(unit_type units) noexcept
    {
        m_bounds.end = add_sat(m_bounds.end, units);
    }

    constexpr void dec_left(unit_type units) noexcept
    {
        m_bounds.start = sub_sat(m_bounds.start, units);
    }

    constexpr void dec_right(unit_type units) noexcept
    {
        m_bounds.end = std::max(sub_sat(m_bounds.end, units), m_bounds.start);
        m_cursor = std::min(m_bounds.end, m_cursor);
    }

    [[nodiscard]] constexpr unit_type position() const noexcept
    {
        return m_cursor;
    }

    [[nodiscard]] constexpr unit_type left() const noexcept
    {
        return m_bounds.start;
    }

    [[nodiscard]] constexpr unit_type right() const noexcept
    {
        return m_bounds.end;
    }

    [[nodiscard]] constexpr unit_type size() const noexcept
    {
        return m_bounds.size();
    }

private:
    bounds_type m_bounds{};
    unit_type m_cursor{};
};

} // namespace flow
