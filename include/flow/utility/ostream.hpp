#pragma once

#include <functional>
#include <ostream>
#include <ranges>

#include "concepts.hpp"
#include "traits.hpp"

namespace flow {

class ostream_view;

template<typename T>
struct serializer
{
    void operator()(ostream_view& out, const T& data) const = delete;
};

class ostream_view
{
public:
    ostream_view(std::ostream& out)
        : m_out{ out }
    {}

    template<typename T, std::invocable<make_lval_ref_t<ostream_view>, make_const_lval_ref_t<T>> SerializerT>
    ostream_view& write(const T& data, SerializerT serializer)
    {
        std::invoke(serializer, *this, data);

        return *this;
    }

    template<typename T>
    ostream_view& write(const T& data)
    {
        return write(data, serializer<T>{});
    }

    [[nodiscard]] std::ostream& get() noexcept
    {
        return m_out;
    }

private:
    std::ostream& m_out;
};

template<concepts::trivially_copyable T>
struct serializer<T>
{
    void operator()(ostream_view& out, const T& value) const
    {
        out.get().write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
};

template<concepts::trivially_copyable_range R>
struct serializer<R>
{
    void operator()(ostream_view& out, const R& range) const
    {
        out.get().write(reinterpret_cast<const char*>(std::ranges::cdata(range)),
                        std::ranges::size(range) * sizeof(std::ranges::range_value_t<R>));
    }
};

template<std::ranges::range R>
struct serializer<R>
{
    void operator()(ostream_view& out, const R& range) const
    {
        for (const auto& e : range | std::views::all)
        {
            out.write(e);
        }
    }
};

} // namespace flow
