#pragma once

#include <functional>
#include <istream>
#include <ranges>

#include "concepts.hpp"
#include "serialization.hpp"

namespace flow {

class istream_view
{
public:
    istream_view(std::istream& in)
        : m_in{ &in }
    {}

    template<concepts::trivially_copyable_non_range T>
    istream_view& read(T& data)
    {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        m_in->read(reinterpret_cast<char*>(&data), sizeof(data));

        return *this;
    }

    template<concepts::trivially_copyable_range_data R>
    istream_view& read(R& range)
    {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        m_in->read(reinterpret_cast<char*>(std::ranges::data(range)),
                   std::ranges::size(range) * sizeof(std::ranges::range_value_t<R>));

        return *this;
    }

    template<typename T, concepts::deserializer<T> DeserializerT>
    istream_view& deserialize(T& data, DeserializerT deserializer)
    {
        std::invoke(deserializer, *this, data);

        return *this;
    }

    template<typename T, typename Traits = serialization_traits<T>>
    istream_view& deserialize(T& data)
    {
        return deserialize(data, deserializer<T, Traits>{});
    }

private:
    std::istream* m_in;
};

template<concepts::trivially_copyable_non_range T, typename Traits>
struct deserializer<T, Traits>
{
    void operator()(istream_view& in, T& data) const
    {
        in.read(data);
    }
};

template<concepts::resizable_range R, typename Traits>
struct deserializer<R, Traits>
{
    void operator()(istream_view& in, R& range) const
    {
        using size_type = typename Traits::size_type;
        size_type size{};

        in.read(size);

        range.resize(size);

        if constexpr (concepts::trivially_copyable_range_data<R>)
        {
            in.read(range);
        }
        else
        {
            for (auto& e : range)
            {
                in.deserialize(e);
            }
        }
    }
};

} // namespace flow
