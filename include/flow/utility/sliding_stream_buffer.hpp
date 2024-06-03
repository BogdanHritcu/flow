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
                                    size_type element_begin_index,
                                    size_type element_count,
                                    size_type buffer_element_count)
        : m_sliding_window(element_begin_index,
                           element_count,
                           buffer_element_count)
        , m_stream_start_position{ stream_start_position }
    {
        m_buffer.resize(m_sliding_window.size());
    }

    constexpr sliding_stream_buffer(size_type stream_start_position,
                                    size_type element_begin_index,
                                    size_type element_count,
                                    size_type buffer_element_count,
                                    size_type buffer_begin_index)
        : m_sliding_window(element_begin_index,
                           element_count,
                           buffer_element_count,
                           buffer_begin_index)
        , m_stream_start_position{ stream_start_position }
    {
        m_buffer.resize(m_sliding_window.size());
    }

    constexpr void create(size_type stream_start_position,
                          size_type element_begin_index,
                          size_type element_count,
                          size_type buffer_element_count)
    {
        m_stream_start_position = stream_start_position;

        m_sliding_window = sliding_window(element_begin_index,
                                          element_count,
                                          buffer_element_count);

        m_buffer.resize(m_sliding_window.size());
    }

    constexpr void create(size_type stream_start_position,
                          size_type element_begin_index,
                          size_type element_count,
                          size_type buffer_element_count,
                          size_type buffer_begin_index)
    {
        m_stream_start_position = stream_start_position;

        m_sliding_window = sliding_window(element_begin_index,
                                          element_count,
                                          buffer_element_count,
                                          buffer_begin_index);

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

    constexpr void set_begin(size_type position) noexcept
    {
        m_sliding_window.seek(position);
    }

    [[nodiscard]] constexpr size_type begin() const noexcept
    {
        return m_sliding_window.begin();
    }

    [[nodiscard]] constexpr size_type end() const noexcept
    {
        return m_sliding_window.end();
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return m_sliding_window.size();
    }

    [[nodiscard]] constexpr size_type loaded_size() const noexcept
    {
        return m_buffer.size();
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

    [[nodiscard]] constexpr size_type stream_begin() const noexcept
    {
        return m_stream_start_position + begin() * sizeof(value_type);
    }

    [[nodiscard]] constexpr size_type stream_end() const noexcept
    {
        return m_stream_start_position + end() * sizeof(value_type);
    }

    [[nodiscard]] constexpr size_type stream_size() const noexcept
    {
        return size() * sizeof(value_type);
    }

    [[nodiscard]] constexpr size_type stream_bounds_begin() const noexcept
    {
        return m_stream_start_position;
    }

    [[nodiscard]] constexpr size_type stream_bounds_end() const noexcept
    {
        return m_stream_start_position + stream_bounds_size();
    }

    [[nodiscard]] constexpr size_type stream_bounds_size() const noexcept
    {
        return bounds_size() * sizeof(value_type);
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

    void load(istream_view in)
    {
        in.seek(stream_begin());
        in.read(values());
    }

    void save(ostream_view out) const
    {
        out.seek(stream_begin());
        out.write(values());
    }

private:
    std::vector<value_type> m_buffer{};
    sliding_window<size_type> m_sliding_window{};
    size_type m_stream_start_position{};
};

} // namespace flow
