#pragma once

#include <algorithm>
#include <concepts>

#include "integer_range.hpp"
#include "numeric.hpp"

namespace flow {

namespace detail {

    template<std::unsigned_integral T>
    struct bounded_cursor
    {
        ie_integer_range<T> bounds;
        T position;
    };

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bounded_cursor<T> make_bounded_cursor(T begin, T size, T position) noexcept
    {
        return bounded_cursor<T>{
            .bounds{ begin, add_sat(begin + size) },
            .position{ std::clamp(position, begin, add_sat(begin + size)) }
        };
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bounded_cursor<T> make_bounded_cursor(T begin, T size) noexcept
    {
        return bounded_cursor<T>{
            .bounds{ begin, add_sat(begin + size) },
            .position{ begin }
        };
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_forward(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_position = cursor.position;

        cursor.position = std::min(cursor.bounds.end, add_sat(cursor.position, amount));

        return cursor.position - old_position;
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_backward(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_position = cursor.position;

        cursor.position = std::max(cursor.bounds.begin, sub_sat(cursor.position, amount));

        return old_position - cursor.position;
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_forward_inc(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_position = cursor.position;

        auto capacity = sub_sat(cursor.bounds.end - cursor.position, T{ 1 });
        if (amount > capacity)
        {
            cursor.bounds.end = add_sat(cursor.bounds.end, amount - capacity);
            cursor.position = cursor.bounds.end - 1;
        }
        else
        {
            cursor.position += amount;
        }

        return cursor.position - old_position;
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_backward_dec(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_position = cursor.position;

        cursor.position = sub_sat(cursor.position, amount);
        cursor.bounds.begin = std::min(cursor.position, cursor.bounds.begin);

        return old_position - cursor.position;
    }

    template<std::unsigned_integral T>
    constexpr void bounded_cursor_seek(bounded_cursor<T>& cursor, T position) noexcept
    {
        cursor.position = std::clamp(position, cursor.bounds.begin, cursor.bounds.end);
    }

    template<std::unsigned_integral T>
    constexpr void bounded_cursor_seek_inc(bounded_cursor<T>& cursor, T position) noexcept
    {
        cursor.position = std::max(position, cursor.bounds.begin);
        cursor.bounds.end = std::max(add_sat(cursor.position, T{ 1 }), cursor.bounds.end);
    }

    template<std::unsigned_integral T>
    constexpr void bounded_cursor_seek_dec(bounded_cursor<T>& cursor, T position) noexcept
    {
        cursor.position = std::min(position, cursor.bounds.end);
        cursor.bounds.begin = std::min(cursor.position, cursor.bounds.begin);
    }

    template<std::unsigned_integral T>
    constexpr void bounded_cursor_seek_inc_dec(bounded_cursor<T>& cursor, T position) noexcept
    {
        cursor.position = position;
        cursor.bounds.begin = std::min(cursor.position, cursor.bounds.begin);
        cursor.bounds.end = std::max(add_sat(cursor.position, T{ 1 }), cursor.bounds.end);
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_inc_begin(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_begin = cursor.bounds.begin;

        cursor.bounds.begin = std::min(add_sat(cursor.bounds.begin, amount), cursor.bounds.end);
        cursor.position = std::max(cursor.bounds.begin, cursor.position);

        return cursor.bounds.begin - old_begin;
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_inc_end(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_end = cursor.bounds.end;

        cursor.bounds.end = add_sat(cursor.bounds.end, amount);

        return cursor.bounds.end - old_end;
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_dec_begin(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_begin = cursor.bounds.begin;

        cursor.bounds.begin = sub_sat(cursor.bounds.begin, amount);

        return old_begin - cursor.bounds.begin;
    }

    template<std::unsigned_integral T>
    constexpr T bounded_cursor_dec_end(bounded_cursor<T>& cursor, T amount) noexcept
    {
        const auto old_end = cursor.bounds.end;

        cursor.bounds.end = std::max(sub_sat(cursor.bounds.end, amount), cursor.bounds.begin);
        cursor.position = std::min(cursor.bounds.end, cursor.position);

        return old_end - cursor.bounds.end;
    }

    template<std::unsigned_integral T>
    constexpr void bounded_cursor_set_size(bounded_cursor<T>& cursor, T size) noexcept
    {
        const auto capacity = cursor.bounds.end - cursor.bounds.begin;

        if (size > capacity)
        {
            detail::bounded_cursor_inc_end(cursor, size - capacity);
        }
        else
        {
            detail::bounded_cursor_dec_end(cursor, capacity - size);
        }
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bool bounded_cursor_is_at_begin(const bounded_cursor<T>& cursor) noexcept
    {
        return cursor.position == cursor.bounds.begin;
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bool bounded_cursor_is_at_end(const bounded_cursor<T>& cursor) noexcept
    {
        return cursor.position == cursor.bounds.end;
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bool bounded_cursor_is_valid(const bounded_cursor<T>& cursor) noexcept
    {
        return cursor.bounds.is_valid() && cursor.bounds.contains(cursor.position);
    }

} // namespace detail

template<std::unsigned_integral T>
class bounded_cursor
{
public:
    using unit_type = T;
    using bounds_type = ie_integer_range<unit_type>;

public:
    constexpr bounded_cursor() noexcept = default;

    constexpr bounded_cursor(unit_type begin, unit_type size, unit_type position) noexcept
        : m_cursor{ detail::make_bounded_cursor(begin, size, position) }
    {}

    constexpr bounded_cursor(unit_type begin, unit_type size) noexcept
        : m_cursor{ detail::make_bounded_cursor(begin, size) }
    {}

    constexpr unit_type forward(unit_type amount) noexcept
    {
        return detail::bounded_cursor_forward(m_cursor, amount);
    }

    constexpr unit_type forward_inc(unit_type amount) noexcept
    {
        return detail::bounded_cursor_forward_inc(m_cursor, amount);
    }

    constexpr unit_type backward(unit_type amount) noexcept
    {
        return detail::bounded_cursor_backward(m_cursor, amount);
    }

    constexpr unit_type backward_dec(unit_type amount) noexcept
    {
        return detail::bounded_cursor_backward_dec(m_cursor, amount);
    }

    constexpr void seek(unit_type position) noexcept
    {
        detail::bounded_cursor_seek(m_cursor, position);
    }

    constexpr void seek_inc(unit_type position) noexcept
    {
        detail::bounded_cursor_seek_inc(m_cursor, position);
    }

    constexpr void seek_dec(unit_type position) noexcept
    {
        detail::bounded_cursor_seek_dec(m_cursor, position);
    }

    constexpr void seek_inc_dec(unit_type position) noexcept
    {
        detail::bounded_cursor_seek_inc_dec(m_cursor, position);
    }

    constexpr unit_type inc_begin(unit_type amount) noexcept
    {
        return detail::bounded_cursor_inc_begin(m_cursor, amount);
    }

    constexpr unit_type inc_end(unit_type amount) noexcept
    {
        return detail::bounded_cursor_inc_end(m_cursor, amount);
    }

    constexpr unit_type dec_begin(unit_type amount) noexcept
    {
        return detail::bounded_cursor_dec_begin(m_cursor, amount);
    }

    constexpr unit_type dec_end(unit_type amount) noexcept
    {
        return detail::bounded_cursor_dec_end(m_cursor, amount);
    }

    constexpr void set_size(unit_type size) noexcept
    {
        detail::bounded_cursor_set_size(m_cursor, size);
    }

    [[nodiscard]] constexpr bounds_type bounds() const noexcept
    {
        return m_cursor.bounds;
    }

    [[nodiscard]] constexpr unit_type position() const noexcept
    {
        return m_cursor.position;
    }

    [[nodiscard]] constexpr unit_type begin() const noexcept
    {
        return m_cursor.bounds.begin;
    }

    [[nodiscard]] constexpr unit_type end() const noexcept
    {
        return m_cursor.bounds.end;
    }

    [[nodiscard]] constexpr bool is_at_begin() const noexcept
    {
        return detail::bounded_cursor_is_at_begin(m_cursor);
    }

    [[nodiscard]] constexpr bool is_at_end() const noexcept
    {
        return detail::bounded_cursor_is_at_end(m_cursor);
    }

    [[nodiscard]] constexpr operator bool() const noexcept
    {
        return !is_at_end();
    }

private:
    detail::bounded_cursor<unit_type> m_cursor;
};

} // namespace flow
