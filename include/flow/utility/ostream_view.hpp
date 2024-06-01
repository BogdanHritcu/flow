#pragma once

#include <functional>
#include <ostream>
#include <ranges>
#include <span>

#include "concepts.hpp"
#include "serialization.hpp"

namespace flow {

class ostream_view
{
public:
    using pos_type = std::ostream::pos_type;
    using off_type = std::ostream::off_type;
    using seekdir = std::ostream::seekdir;
    using iostate = std::ostream::iostate;

    static constexpr auto goodbit = std::ostream::goodbit;
    static constexpr auto badbit = std::ostream::badbit;
    static constexpr auto failbit = std::ostream::failbit;
    static constexpr auto eofbit = std::ostream::eofbit;

    static constexpr auto begin = std::ostream::beg;
    static constexpr auto end = std::ostream::end;
    static constexpr auto current = std::ostream::cur;

public:
    constexpr ostream_view() noexcept = default;

    constexpr ostream_view(std::ostream* out) noexcept
        : m_out{ out }
    {}

    constexpr ostream_view(std::ostream& out) noexcept
        : ostream_view(&out)
    {}

    template<concepts::trivially_copyable T>
    ostream_view& write(const T& data)
    {
        if (m_out)
        {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            m_out->write(reinterpret_cast<const char*>(&data), sizeof(data));
        }
        return *this;
    }

    template<concepts::trivially_copyable T>
    ostream_view& write(std::span<const T> span)
    {
        if (m_out)
        {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            m_out->write(reinterpret_cast<const char*>(span.data()), span.size_bytes());
        }
        return *this;
    }

    template<typename T, concepts::serializer<T> SerializerT>
    ostream_view& serialize(const T& data, SerializerT serializer)
    {
        if (m_out)
        {
            std::invoke(serializer, *this, data);
        }
        return *this;
    }

    template<typename T, typename Traits = serialization_traits<T>>
    ostream_view& serialize(const T& data)
    {
        return serialize(data, serializer<T, Traits>{});
    }

    ostream_view& seek(pos_type position)
    {
        if (m_out)
        {
            m_out->seekp(position);
        }
        return *this;
    }

    ostream_view& seek(off_type offset, seekdir direction)
    {
        if (m_out)
        {
            m_out->seekp(offset, direction);
        }
        return *this;
    }

    [[nodiscard]] pos_type tell()
    {
        return m_out ? m_out->tellp() : pos_type{ 0 };
    }

    [[nodiscard]] bool good() const
    {
        return m_out && m_out->good();
    }

    [[nodiscard]] bool eof() const
    {
        return m_out && m_out->eof();
    }

    [[nodiscard]] bool fail() const
    {
        return m_out && m_out->fail();
    }

    [[nodiscard]] bool bad() const
    {
        return m_out && m_out->bad();
    }

    [[nodiscard]] bool operator!() const
    {
        return m_out && m_out->fail();
    }

    [[nodiscard]] explicit operator bool() const
    {
        return m_out && !m_out->fail();
    }

    void clear(iostate state = goodbit)
    {
        if (m_out)
        {
            m_out->clear(state);
        }
    }

private:
    std::ostream* m_out{ nullptr };
};

template<concepts::trivially_copyable T, typename Traits>
struct serializer<T, Traits>
{
    void operator()(ostream_view out, const T& data) const
    {
        out.write(data);
    }
};

template<concepts::trivially_copyable_data_sized_range R, typename Traits>
    requires(!concepts::trivially_copyable<R>)
struct serializer<R, Traits>
{
    using size_type = typename Traits::size_type;

    void operator()(ostream_view out, const R& range) const
    {
        const auto size = static_cast<size_type>(std::ranges::size(range));
        out.write(size);
        out.write(std::span{ range | std::views::take(size) });
    }
};

template<concepts::non_trivially_copyable_data_sized_range R, typename Traits>
struct serializer<R, Traits>
{
    using size_type = typename Traits::size_type;

    void operator()(ostream_view out, const R& range) const
    {
        const auto size = static_cast<size_type>(std::ranges::size(range));
        out.write(size);

        for (const auto& e : range | std::views::take(size))
        {
            out.serialize(e);
        }
    }
};

} // namespace flow
