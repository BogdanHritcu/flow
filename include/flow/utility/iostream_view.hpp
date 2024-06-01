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
