#pragma once

#include <functional>
#include <istream>
#include <ranges>

#include "concepts.hpp"
#include "traits.hpp"

namespace flow {

class istream_view;

template<typename T>
struct deserializer
{
    void operator()(istream_view& in, T& data) const = delete;
};

class istream_view
{
public:
    istream_view(std::istream& in)
        : m_in{ in }
    {}

    template<typename T,
             std::invocable<make_lval_ref_t<istream_view>, make_lval_ref_t<T>> DeserializerT>
    istream_view& read(T& data, DeserializerT deserializer)
    {
        std::invoke(deserializer, *this, data);

        return *this;
    }

    template<typename T>
    istream_view& read(T& data)
    {
        return read(data, deserializer<T>{});
    }

    [[nodiscard]] std::istream& get() noexcept
    {
        return m_in;
    }

private:
    std::istream& m_in;
};

template<concepts::trivially_copyable T>
struct deserializer<T>
{
    void operator()(istream_view& in, T& value) const
    {
        in.get().read(reinterpret_cast<char*>(&value), sizeof(value));
    }
};

template<concepts::trivially_copyable_range R>
struct deserializer<R>
{
    void operator()(istream_view& in, R& range) const
    {
        in.get().read(reinterpret_cast<char*>(std::ranges::data(range)),
                      std::ranges::size(range) * sizeof(std::ranges::range_value_t<R>));
    }
};

template<std::ranges::range R>
struct deserializer<R>
{
    void operator()(istream_view& in, R& range) const
    {
        for (auto& e : range | std::views::all)
        {
            in.read(e);
        }
    }
};

} // namespace flow
