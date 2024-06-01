#pragma once

#include <algorithm>
#include <concepts>

#include "bounded_cursor.hpp"

namespace flow {

namespace detail {

    template<std::unsigned_integral T>
    struct sliding_window
    {
        detail::bounded_cursor<T> cursor;
        T size;
    };

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr sliding_window<T> make_sliding_window(T begin,
                                                                  T size,
                                                                  T window_size,
                                                                  T window_begin) noexcept
    {
        window_size = std::min(window_size, size);

        return sliding_window<T>{
            .cursor{ make_bounded_cursor(begin, size - window_size, window_begin) },
            .size{ window_size }
        };
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr sliding_window<T> make_sliding_window(T begin, T size, T window_size) noexcept
    {
        window_size = std::min(window_size, size);

        return sliding_window<T>{
            .cursor{ make_bounded_cursor(begin, size - window_size) },
            .size{ window_size }
        };
    }

    template<std::unsigned_integral T>
    constexpr T sliding_window_forward(sliding_window<T>& window, T amount) noexcept
    {
        return detail::bounded_cursor_forward(window.cursor, amount);
    }

    template<std::unsigned_integral T>
    constexpr T sliding_window_backward(sliding_window<T>& window, T amount) noexcept
    {
        return detail::bounded_cursor_backward(window.cursor, amount);
    }

    template<std::unsigned_integral T>
    constexpr T sliding_window_forward_inc(sliding_window<T>& window, T amount) noexcept
    {
        return detail::bounded_cursor_forward_inc(window.cursor, amount);
    }

    template<std::unsigned_integral T>
    constexpr T sliding_window_backward_dec(sliding_window<T>& window, T amount) noexcept
    {
        return detail::bounded_cursor_backward_dec(window.cursor, amount);
    }

    template<std::unsigned_integral T>
    constexpr void sliding_window_seek(sliding_window<T>& window, T position) noexcept
    {
        detail::bounded_cursor_seek(window.cursor, position);
    }

    template<std::unsigned_integral T>
    constexpr void sliding_window_seek_inc(sliding_window<T>& window, T position) noexcept
    {
        detail::bounded_cursor_seek_inc(window.cursor, position);
    }

    template<std::unsigned_integral T>
    constexpr void sliding_window_seek_dec(sliding_window<T>& window, T position) noexcept
    {
        detail::bounded_cursor_seek_dec(window.cursor, position);
    }

    template<std::unsigned_integral T>
    constexpr void sliding_window_seek_inc_dec(sliding_window<T>& window, T position) noexcept
    {
        detail::bounded_cursor_seek_inc_dec(window.cursor, position);
    }

    template<std::unsigned_integral T>
    constexpr T sliding_window_inc_bounds_end(sliding_window<T>& window, T amount) noexcept
    {
        return detail::bounded_cursor_inc_end(window.cursor, amount);
    }

    template<std::unsigned_integral T>
    constexpr T sliding_window_dec_bounds_begin(sliding_window<T>& window, T amount) noexcept
    {
        return detail::bounded_cursor_dec_begin(window.cursor, amount);
    }

    template<std::unsigned_integral T>
    constexpr void sliding_window_resize_bounds(sliding_window<T>& window, T size) noexcept
    {
        detail::bounded_cursor_resize(window.cursor, size);
        window.size = std::min(window.size, window.cursor.bounds.end - window.cursor.position);
    }

    template<std::unsigned_integral T>
    constexpr void sliding_window_resize(sliding_window<T>& window, T size) noexcept
    {
        const auto capacity = window.cursor.bounds.end - window.cursor.position;

        if (size > capacity)
        {
            window.size += detail::bounded_cursor_inc_end(window.cursor, size - capacity);
        }
        else
        {
            window.size = size;
        }
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr T sliding_window_bounds_size(const sliding_window<T>& window) noexcept
    {
        return detail::bounded_cursor_size(window.cursor);
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bool sliding_window_is_at_begin(const sliding_window<T>& window) noexcept
    {
        return detail::bounded_cursor_is_at_begin(window.cursor);
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bool sliding_window_is_at_end(const sliding_window<T>& window) noexcept
    {
        return detail::bounded_cursor_is_at_end(window.cursor);
    }

    template<std::unsigned_integral T>
    [[nodiscard]] constexpr bool sliding_window_is_valid(const sliding_window<T>& window) noexcept
    {
        return detail::bounded_cursor_is_valid(window.cursor)
            && window.size <= window.cursor.bounds.end - window.cursor.position;
    }

} // namespace detail

template<std::unsigned_integral T>
class sliding_window
{
public:
    using unit_type = T;
    using bounds_type = ie_integer_range<unit_type>;

public:
    constexpr sliding_window() noexcept = default;

    constexpr sliding_window(unit_type begin, unit_type size, unit_type window_size, unit_type window_begin) noexcept
        : m_window{ detail::make_sliding_window(begin, size, window_size, window_begin) }
    {}

    constexpr sliding_window(unit_type begin, unit_type size, unit_type window_size) noexcept
        : m_window{ detail::make_sliding_window(begin, size, window_size) }
    {}

    constexpr unit_type forward(unit_type amount) noexcept
    {
        return detail::sliding_window_forward(m_window, amount);
    }

    constexpr unit_type forward_inc(unit_type amount) noexcept
    {
        return detail::sliding_window_forward_inc(m_window, amount);
    }

    constexpr unit_type backward(unit_type amount) noexcept
    {
        return detail::sliding_window_backward(m_window, amount);
    }

    constexpr unit_type backward_dec(unit_type amount) noexcept
    {
        return detail::sliding_window_backward_dec(m_window, amount);
    }

    constexpr void seek(unit_type position) noexcept
    {
        detail::sliding_window_seek(m_window, position);
    }

    constexpr void seek_inc(unit_type position) noexcept
    {
        detail::sliding_window_seek_inc(m_window, position);
    }

    constexpr void seek_dec(unit_type position) noexcept
    {
        detail::sliding_window_seek_dec(m_window, position);
    }

    constexpr void seek_inc_dec(unit_type position) noexcept
    {
        detail::sliding_window_seek_inc_dec(m_window, position);
    }

    constexpr unit_type inc_bounds_end(unit_type amount) noexcept
    {
        return detail::sliding_window_inc_bounds_end(m_window, amount);
    }

    constexpr unit_type dec_bounds_begin(unit_type amount) noexcept
    {
        return detail::sliding_window_dec_bounds_begin(m_window, amount);
    }

    constexpr void resize(unit_type size) noexcept
    {
        return detail::sliding_window_resize(m_window, size);
    }

    constexpr void resize_bounds(unit_type size) noexcept
    {
        return detail::sliding_window_resize_bounds(m_window, size);
    }

    [[nodiscard]] constexpr unit_type begin() const noexcept
    {
        return m_window.cursor.bounds.begin;
    }

    [[nodiscard]] constexpr unit_type end() const noexcept
    {
        return m_window.cursor.position + m_window.size;
    }

    [[nodiscard]] constexpr unit_type position() const noexcept
    {
        return m_window.cursor.position;
    }

    [[nodiscard]] constexpr unit_type bounds_begin() const noexcept
    {
        return m_window.cursor.bounds.begin;
    }

    [[nodiscard]] constexpr unit_type bounds_end() const noexcept
    {
        return m_window.cursor.bounds.end + m_window.size;
    }

    [[nodiscard]] constexpr unit_type size() const noexcept
    {
        return m_window.size;
    }

    [[nodiscard]] constexpr unit_type bounds_size() const noexcept
    {
        return detail::sliding_window_bounds_size(m_window);
    }

    [[nodiscard]] constexpr bool is_at_begin() const noexcept
    {
        return detail::sliding_window_is_at_begin(m_window);
    }

    [[nodiscard]] constexpr bool is_at_end() const noexcept
    {
        return detail::sliding_window_is_at_end(m_window);
    }

    [[nodiscard]] constexpr operator bool() const noexcept
    {
        return !is_at_end();
    }

private:
    detail::sliding_window<unit_type> m_window;
};

} // namespace flow
