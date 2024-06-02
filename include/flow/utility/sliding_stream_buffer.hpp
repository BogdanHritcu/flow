#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "concepts.hpp"
#include "istream_view.hpp"
#include "ostream_view.hpp"
#include "sliding_window.hpp"

namespace flow {

template<concepts::trivially_copyable T>
class sliding_stream_buffer
{
public:
    using value_type = T;
    using size_type = std::size_t;

public:
    constexpr sliding_stream_buffer() noexcept = default;

    constexpr sliding_stream_buffer(size_type begin,
                                    size_type size,
                                    size_type window_size)
        : m_sliding_window(begin, size, window_size)
    {
        m_buffer.resize(m_sliding_window.size());
    }

    constexpr sliding_stream_buffer(size_type begin,
                                    size_type size,
                                    size_type window_size,
                                    size_type window_begin)
        : m_sliding_window(begin, size, window_size, window_begin)
    {
        m_buffer.resize(m_sliding_window.size());
    }

    constexpr size_type forward(size_type amount) noexcept
    {
        return m_sliding_window.forward(amount);
    }

    constexpr size_type backward(size_type amount) noexcept
    {
        return m_sliding_window.backward(amount);
    }

    constexpr size_type forward_inc(size_type amount) noexcept
    {
        return m_sliding_window.forward_inc(amount);
    }

    constexpr size_type backward_dec(size_type amount) noexcept
    {
        return m_sliding_window.backward_dec(amount);
    }

    constexpr void set_position(size_type position) noexcept
    {
        m_sliding_window.seek(position);
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return m_buffer.size();
    }

    [[nodiscard]] constexpr size_type position() const noexcept
    {
        return m_sliding_window.position();
    }

    [[nodiscard]] constexpr size_type bounds_begin() const noexcept
    {
        return m_sliding_window.bounds_begin();
    }

    [[nodiscard]] constexpr size_type bounds_end() const noexcept
    {
        return m_sliding_window.bounds_end();
    }

    [[nodiscard]] constexpr size_type bounds_size() const noexcept
    {
        return m_sliding_window.bounds_size();
    }

    [[nodiscard]] constexpr std::span<value_type> values() noexcept
    {
        return m_buffer;
    }

    [[nodiscard]] constexpr std::span<const value_type> values() const noexcept
    {
        return m_buffer;
    }

    constexpr void resize(size_type size)
    {
        m_sliding_window.resize(size);
        m_buffer.resize(m_sliding_window.size());
    }

    void load(istream_view in, size_type begin_offset = 0)
    {
        in.seek(m_sliding_window.position() + begin_offset);
        in.read(m_buffer);
    }

    void save(ostream_view out, size_type begin_offset = 0)
    {
        out.seek(m_sliding_window.position() + begin_offset);
        out.write(m_buffer);
    }

private:
    std::vector<value_type> m_buffer{};
    sliding_window<size_type> m_sliding_window{};
};

} // namespace flow
