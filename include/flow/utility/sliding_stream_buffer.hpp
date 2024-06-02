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

    constexpr sliding_stream_buffer(size_type stream_start_position,
                                    size_type total_element_count,
                                    size_type buffer_element_count)
        : m_sliding_window(0,
                           total_element_count,
                           buffer_element_count)
        , m_stream_start_position{ stream_start_position }
    {
        m_buffer.resize(m_sliding_window.size());
    }

    constexpr sliding_stream_buffer(size_type stream_start_position,
                                    size_type total_element_count,
                                    size_type buffer_element_count,
                                    size_type buffer_begin_index)
        : m_sliding_window(0,
                           total_element_count,
                           buffer_element_count,
                           buffer_begin_index)
        , m_stream_start_position{ stream_start_position }
    {
        m_buffer.resize(m_sliding_window.size());
    }

    constexpr size_type forward(size_type count) noexcept
    {
        return m_sliding_window.forward(count);
    }

    constexpr size_type backward(size_type count) noexcept
    {
        return m_sliding_window.backward(count);
    }

    constexpr size_type forward_inc(size_type count) noexcept
    {
        return m_sliding_window.forward_inc(count);
    }

    constexpr size_type backward_dec(size_type count) noexcept
    {
        return m_sliding_window.backward_dec(count);
    }

    constexpr void set_position(size_type position) noexcept
    {
        m_sliding_window.seek(position);
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return m_buffer.size();
    }

    [[nodiscard]] constexpr size_type size_bytes() const noexcept
    {
        return m_buffer.size() * sizeof(value_type);
    }

    [[nodiscard]] constexpr size_type position() const noexcept
    {
        return m_sliding_window.position();
    }

    [[nodiscard]] constexpr size_type stream_position() const noexcept
    {
        return m_stream_start_position + m_sliding_window.position() * sizeof(value_type);
    }

    [[nodiscard]] constexpr size_type stream_begin() const noexcept
    {
        return m_stream_start_position;
    }

    [[nodiscard]] constexpr size_type stream_end() const noexcept
    {
        return m_stream_start_position + stream_size();
    }

    [[nodiscard]] constexpr size_type stream_size() const noexcept
    {
        return m_sliding_window.bounds_size() * sizeof(value_type);
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

    void load(istream_view in, size_type stream_byte_offset = 0)
    {
        in.seek(stream_position() + stream_byte_offset);
        in.read(values());
    }

    void save(ostream_view out, size_type stream_byte_offset = 0) const
    {
        out.seek(stream_position() + stream_byte_offset);
        out.write(values());
    }

private:
    std::vector<value_type> m_buffer{};
    sliding_window<size_type> m_sliding_window{};
    size_type m_stream_start_position{};
};

} // namespace flow
