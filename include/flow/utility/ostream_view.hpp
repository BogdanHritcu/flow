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
    constexpr ostream_view() noexcept = default;

    ostream_view(std::ostream& out) noexcept
        : m_out{ &out }
    {}

    template<concepts::trivially_copyable T>
    ostream_view& write(const T& data)
    {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        m_out->write(reinterpret_cast<const char*>(&data), sizeof(data));

        return *this;
    }

    template<concepts::trivially_copyable T>
    ostream_view& write(std::span<const T> span)
    {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        m_out->write(reinterpret_cast<const char*>(span.data()), span.size_bytes());

        return *this;
    }

    template<typename T, concepts::serializer<T> SerializerT>
    ostream_view& serialize(const T& data, SerializerT serializer)
    {
        std::invoke(serializer, *this, data);

        return *this;
    }

    template<typename T, typename Traits = serialization_traits<T>>
    ostream_view& serialize(const T& data)
    {
        return serialize(data, serializer<T, Traits>{});
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
