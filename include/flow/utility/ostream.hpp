#pragma once

#include <functional>
#include <ostream>
#include <ranges>

#include "concepts.hpp"
#include "serialization.hpp"

namespace flow {

class ostream_view
{
public:
    ostream_view(std::ostream& out)
        : m_out{ &out }
    {}

    template<concepts::trivially_copyable_non_range T>
    ostream_view& write(const T& data)
    {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        m_out->write(reinterpret_cast<const char*>(&data), sizeof(data));

        return *this;
    }

    template<concepts::trivially_copyable_range_data R>
    ostream_view& write(const R& range)
    {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        m_out->write(reinterpret_cast<const char*>(std::ranges::cdata(range)),
                     std::ranges::size(range) * sizeof(std::ranges::range_value_t<R>));

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
    std::ostream* m_out;
};

template<concepts::trivially_copyable_non_range T, typename Traits>
struct serializer<T, Traits>
{
    void operator()(ostream_view& out, const T& data) const
    {
        out.write(data);
    }
};

template<std::ranges::sized_range R, typename Traits>
struct serializer<R, Traits>
{
    void operator()(ostream_view& out, const R& range) const
    {
        using size_type = typename Traits::size_type;

        const auto size = static_cast<size_type>(std::ranges::size(range));

        out.write(size);

        if constexpr (concepts::trivially_copyable_range_data<R>)
        {
            out.write(range | std::views::take(size));
        }
        else
        {
            for (const auto& e : range | std::views::take(size))
            {
                out.serialize(e);
            }
        }
    }
};

} // namespace flow
