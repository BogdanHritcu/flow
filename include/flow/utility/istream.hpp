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
        : m_in{ in }
    {}

    template<concepts::trivially_copyable T>
    istream_view& read(T& data)
    {
        m_in.read(reinterpret_cast<char*>(&data), sizeof(data));

        return *this;
    }

    template<concepts::trivially_copyable_range R>
    istream_view& read(R& range)
    {
        m_in.read(reinterpret_cast<char*>(std::ranges::data(range)),
                  std::ranges::size(range) * sizeof(std::ranges::range_value_t<R>));

        return *this;
    }

    template<typename T, concepts::deserializer DeserializerT>
    istream_view& deserialize(T& data, DeserializerT deserializer)
    {
        std::invoke(deserializer, *this, data);

        return *this;
    }

    template<typename T>
    istream_view& deserialize(T& data)
    {
        return deserialize(data, deserializer<T>{});
    }

private:
    std::istream& m_in;
};

template<concepts::trivially_copyable T>
struct deserializer<T>
{
    void operator()(istream_view& in, T& data) const
    {
        in.read(data);
    }
};

template<concepts::resizable_range R>
struct deserializer<R>
{
    void operator()(istream_view& in, R& range) const
    {
        using traits = serialization_traits<R>;

        typename traits::size_type size{};

        in.read(size);

        range.resize(size);

        if constexpr (concepts::trivially_copyable_range<R>)
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
