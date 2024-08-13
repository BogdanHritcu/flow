#pragma once

#include <functional>
#include <ostream>
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
        if (good())
        {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            m_out->write(reinterpret_cast<const char*>(&data), sizeof(data));
        }
        return *this;
    }

    template<concepts::trivially_copyable T>
    ostream_view& write(std::span<T> span)
    {
        if (good())
        {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            m_out->write(reinterpret_cast<const char*>(span.data()), span.size_bytes());
        }
        return *this;
    }

    template<concepts::trivially_copyable T>
    ostream_view& write(std::span<const T> span)
    {
        if (good())
        {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            m_out->write(reinterpret_cast<const char*>(span.data()), span.size_bytes());
        }
        return *this;
    }

    template<typename T, concepts::serializer<T> SerializerT>
    ostream_view& serialize(const T& data, SerializerT serializer)
    {
        if (good())
        {
            std::invoke(serializer, *this, data);
        }
        return *this;
    }

    template<typename T>
    ostream_view& serialize(const T& data)
    {
        return serialize(data, serializer<T>{});
    }

    template<typename T, concepts::serializer<T> SerializerT>
    ostream_view& serialize(std::span<const T> span, SerializerT serializer)
    {
        if (good())
        {
            std::invoke(serializer, *this, span);
        }
        return *this;
    }

    template<typename T>
    ostream_view& serialize(std::span<const T> span)
    {
        return serialize(span, serializer<T>{});
    }

    ostream_view& seek(pos_type position)
    {
        if (good())
        {
            m_out->seekp(position);
        }
        return *this;
    }

    ostream_view& seek(off_type offset, seekdir direction)
    {
        if (good())
        {
            m_out->seekp(offset, direction);
        }
        return *this;
    }

    [[nodiscard]] pos_type tell()
    {
        return good() ? m_out->tellp() : pos_type{ 0 };
    }

    [[nodiscard]] bool good() const
    {
        return m_out && m_out->good();
    }

    [[nodiscard]] bool eof() const
    {
        return !m_out || m_out->eof();
    }

    [[nodiscard]] bool fail() const
    {
        return !m_out || m_out->fail();
    }

    [[nodiscard]] bool bad() const
    {
        return !m_out || m_out->bad();
    }

    [[nodiscard]] bool operator!() const
    {
        return fail();
    }

    [[nodiscard]] explicit operator bool() const
    {
        return !fail();
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

template<concepts::trivially_copyable T>
struct serializer<T>
{
    void operator()(ostream_view out, const T& data) const
    {
        out.write(data);
    }
};

template<typename T>
struct serializer<std::span<const T>>
{
    void operator()(ostream_view out, std::span<const T> span) const
    {
        if (concepts::trivially_copyable<T>)
        {
            out.write(span);
        }
        else
        {
            for (const auto& e : span)
            {
                out.serialize(e);
            }
        }
    }
};

} // namespace flow
