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

    constexpr bounded_cursor(unit_type begin, unit_type end, unit_type position) noexcept
        : m_bounds{ std::min(begin, end), std::max(begin, end) }
        , m_position{ std::clamp(position, begin, end) }
    {}

    constexpr bounded_cursor(unit_type begin, unit_type end) noexcept
        : bounded_cursor(begin, end, begin)
    {}

    constexpr unit_type forward(unit_type units) noexcept
    {
        const auto old_position = m_position;
        m_position = std::min(m_bounds.end, add_sat(m_position, units));

        return m_position - old_position;
    }

    constexpr unit_type backward(unit_type units) noexcept
    {
        const auto old_position = m_position;
        m_position = std::max(m_bounds.begin, sub_sat(m_position, units));

        return old_position - m_position;
    }

    constexpr unit_type forward_inc(unit_type units) noexcept
    {
        const auto old_position = m_position;

        auto capacity = sub_sat(m_bounds.end - m_position, unit_type{ 1 });
        if (units > capacity)
        {
            m_bounds.end = add_sat(m_bounds.end, units - capacity);
            m_position = m_bounds.end - 1;
        }
        else
        {
            m_position += units;
        }

        return m_position - old_position;
    }

    constexpr unit_type backward_dec(unit_type units) noexcept
    {
        const auto old_position = m_position;
        m_position = sub_sat(m_position, units);
        m_bounds.begin = std::min(m_position, m_bounds.begin);

        return old_position - m_position;
    }

    constexpr unit_type seek(unit_type position) noexcept
    {
        m_position = std::clamp(position, m_bounds.begin, m_bounds.end);

        return m_position;
    }

    constexpr unit_type seek_inc(unit_type position) noexcept
    {
        m_position = std::max(position, m_bounds.begin);
        m_bounds.end = std::max(add_sat(m_position, unit_type{ 1 }), m_bounds.end);

        return m_position;
    }

    constexpr unit_type seek_dec(unit_type position) noexcept
    {
        m_position = std::min(position, m_bounds.end);
        m_bounds.begin = std::min(m_position, m_bounds.begin);

        return m_position;
    }

    constexpr unit_type seek_inc_dec(unit_type position) noexcept
    {
        m_position = position;
        m_bounds.begin = std::min(m_position, m_bounds.begin);
        m_bounds.end = std::max(add_sat(m_position, unit_type{ 1 }), m_bounds.end);

        return m_position;
    }

    constexpr unit_type inc_begin(unit_type units) noexcept
    {
        const auto old_begin = m_bounds.begin;
        m_bounds.begin = std::min(add_sat(m_bounds.begin, units), m_bounds.end);
        m_position = std::max(m_bounds.begin, m_position);

        return m_bounds.begin - old_begin;
    }

    constexpr unit_type inc_end(unit_type units) noexcept
    {
        const auto old_end = m_bounds.end;
        m_bounds.end = add_sat(m_bounds.end, units);

        return m_bounds.end - old_end;
    }

    constexpr unit_type dec_begin(unit_type units) noexcept
    {
        const auto old_begin = m_bounds.begin;
        m_bounds.begin = sub_sat(m_bounds.begin, units);

        return old_begin - m_bounds.begin;
    }

    constexpr unit_type dec_end(unit_type units) noexcept
    {
        const auto old_end = m_bounds.end;
        m_bounds.end = std::max(sub_sat(m_bounds.end, units), m_bounds.begin);
        m_position = std::min(m_bounds.end, m_position);

        return old_end - m_bounds.end;
    }

    [[nodiscard]] constexpr const bounds_type& bounds() const noexcept
    {
        return m_bounds;
    }

    [[nodiscard]] constexpr unit_type position() const noexcept
    {
        return m_position;
    }

    [[nodiscard]] constexpr unit_type begin() const noexcept
    {
        return m_bounds.begin;
    }

    [[nodiscard]] constexpr unit_type end() const noexcept
    {
        return m_bounds.end;
    }

    [[nodiscard]] constexpr unit_type size() const noexcept
    {
        return m_bounds.end - m_bounds.begin;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
        return m_bounds.contains(m_position);
    }

    [[nodiscard]] constexpr operator bool() const noexcept
    {
        return is_valid();
    }

private:
    bounds_type m_bounds{};
    unit_type m_position{};
};

} // namespace flow
