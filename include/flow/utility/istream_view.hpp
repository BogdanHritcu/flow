#pragma once

#include <functional>
#include <ios>
#include <istream>
#include <span>

#include "concepts.hpp"
#include "serialization.hpp"

namespace flow {

class istream_view
{
public:
    using pos_type = std::istream::pos_type;
    using off_type = std::istream::off_type;
    using seekdir = std::istream::seekdir;

    static constexpr auto begin = std::istream::beg;
    static constexpr auto end = std::istream::end;
    static constexpr auto current = std::istream::cur;

public:
    constexpr istream_view() noexcept = default;

    constexpr istream_view(std::istream* in) noexcept
        : m_in{ in }
    {}

    constexpr istream_view(std::istream& in) noexcept
        : istream_view(&in)
    {}

    template<concepts::trivially_copyable T>
    istream_view& read(T& data)
    {
        if (m_in)
        {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            m_in->read(reinterpret_cast<char*>(&data), sizeof(data));
        }
        return *this;
    }

    template<concepts::trivially_copyable T>
    istream_view& read(std::span<T> span)
    {
        if (m_in)
        {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            m_in->read(reinterpret_cast<char*>(span.data()), span.size_bytes());
        }
        return *this;
    }

    template<typename T, concepts::deserializer<T> DeserializerT>
    istream_view& deserialize(T& data, DeserializerT deserializer)
    {
        if (m_in)
        {
            std::invoke(deserializer, *this, data);
        }
        return *this;
    }

    template<typename T, typename Traits = serialization_traits<T>>
    istream_view& deserialize(T& data)
    {
        return deserialize(data, deserializer<T, Traits>{});
    }

    istream_view& seek(pos_type position)
    {
        if (m_in)
        {
            m_in->seekg(position);
        }
        return *this;
    }

    istream_view& seek(off_type offset, seekdir direction)
    {
        if (m_in)
        {
            m_in->seekg(offset, direction);
        }
        return *this;
    }

    [[nodiscard]] pos_type tell()
    {
        return m_in ? m_in->tellg() : 0;
    }

private:
    std::istream* m_in{ nullptr };
};

template<concepts::trivially_copyable T, typename Traits>
struct deserializer<T, Traits>
{
    void operator()(istream_view in, T& data) const
    {
        in.read(data);
    }
};

template<concepts::trivially_copyable_data_resizable_range R, typename Traits>
    requires(!concepts::trivially_copyable<R>)
struct deserializer<R, Traits>
{
    using size_type = typename Traits::size_type;

    void operator()(istream_view in, R& range) const
    {
        size_type size{};
        in.read(size);

        range.resize(size);
        in.read(std::span{ range });
    }
};

template<concepts::non_trivially_copyable_data_resizable_range R, typename Traits>
struct deserializer<R, Traits>
{
    using size_type = typename Traits::size_type;

    void operator()(istream_view in, R& range) const
    {
        size_type size{};
        in.read(size);

        range.resize(size);

        for (auto& e : range)
        {
            in.deserialize(e);
        }
    }
};

} // namespace flow
