#pragma once

#include <istream>
#include <span>

#include "concepts.hpp"
#include "istream_view.hpp"
#include "ostream_view.hpp"

namespace flow {

class iostream_view
{
public:
    using pos_type = std::iostream::pos_type;
    using off_type = std::iostream::off_type;
    using seekdir = std::iostream::seekdir;
    using iostate = std::iostream::iostate;

    static constexpr auto goodbit = std::iostream::goodbit;
    static constexpr auto badbit = std::iostream::badbit;
    static constexpr auto failbit = std::iostream::failbit;
    static constexpr auto eofbit = std::iostream::eofbit;

    static constexpr auto begin = std::iostream::beg;
    static constexpr auto end = std::iostream::end;
    static constexpr auto current = std::iostream::cur;

public:
    constexpr iostream_view() noexcept = default;

    constexpr iostream_view(std::iostream* in_out) noexcept
        : m_in_out{ in_out }
    {}

    constexpr iostream_view(std::iostream& in_out) noexcept
        : iostream_view(&in_out)
    {}

    template<concepts::trivially_copyable T>
    iostream_view& read(T& data)
    {
        istream_view(m_in_out).read(data);

        return *this;
    }

    template<concepts::trivially_copyable T>
    iostream_view& read(std::span<T> span)
    {
        istream_view(m_in_out).read(span);

        return *this;
    }

    template<concepts::trivially_copyable T>
    iostream_view& write(const T& data)
    {
        ostream_view(m_in_out).write(data);

        return *this;
    }

    template<concepts::trivially_copyable T>
    iostream_view& write(std::span<T> span)
    {
        ostream_view(m_in_out).write(span);

        return *this;
    }

    template<concepts::trivially_copyable T>
    iostream_view& write(std::span<const T> span)
    {
        ostream_view(m_in_out).write(span);

        return *this;
    }

    template<typename T, concepts::serializer<T> SerializerT>
    iostream_view& serialize(const T& data, SerializerT serializer)
    {
        ostream_view(m_in_out).serialize(data, serializer);

        return *this;
    }

    template<typename T, typename Traits = serialization_traits<T>>
    iostream_view& serialize(const T& data)
    {
        return serialize(data, serializer<T, Traits>{});
    }

    template<typename T, concepts::deserializer<T> DeserializerT>
    iostream_view& deserialize(T& data, DeserializerT deserializer)
    {
        istream_view(m_in_out).deserialize(data, deserializer);

        return *this;
    }

    template<typename T, typename Traits = serialization_traits<T>>
    iostream_view& deserialize(T& data)
    {
        return deserialize(data, deserializer<T, Traits>{});
    }

    iostream_view& seekg(pos_type position)
    {
        istream_view(m_in_out).seek(position);
        return *this;
    }

    iostream_view& seekg(off_type offset, seekdir direction)
    {
        istream_view(m_in_out).seek(offset, direction);
        return *this;
    }

    iostream_view& seekp(pos_type position)
    {
        ostream_view(m_in_out).seek(position);
        return *this;
    }

    iostream_view& seekp(off_type offset, seekdir direction)
    {
        ostream_view(m_in_out).seek(offset, direction);
        return *this;
    }

    [[nodiscard]] pos_type tellg()
    {
        return istream_view(m_in_out).tell();
    }

    [[nodiscard]] pos_type tellp()
    {
        return ostream_view(m_in_out).tell();
    }

    [[nodiscard]] bool good() const
    {
        return m_in_out && m_in_out->good();
    }

    [[nodiscard]] bool eof() const
    {
        return !m_in_out || m_in_out->eof();
    }

    [[nodiscard]] bool fail() const
    {
        return !m_in_out || m_in_out->fail();
    }

    [[nodiscard]] bool bad() const
    {
        return !m_in_out || m_in_out->bad();
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
        if (m_in_out)
        {
            m_in_out->clear(state);
        }
    }

    [[nodiscard]] constexpr operator istream_view() const noexcept
    {
        return { m_in_out };
    }

    [[nodiscard]] constexpr operator ostream_view() const noexcept
    {
        return { m_in_out };
    }

private:
    std::iostream* m_in_out{ nullptr };
};

} // namespace flow
