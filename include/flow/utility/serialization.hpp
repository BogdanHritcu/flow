#pragma once

#include <ranges>

#include "traits.hpp"

namespace flow {

class istream_view;
class ostream_view;

namespace concepts {

    template<typename F, typename T>
    concept serializer = std::invocable<F, make_lval_ref_t<ostream_view>, make_const_lval_ref_t<T>>;

    template<typename F, typename T>
    concept deserializer = std::invocable<F, make_lval_ref_t<istream_view>, make_lval_ref_t<T>>;

} // namespace concepts

template<typename T>
struct serialization_traits
{};

template<std::ranges::sized_range R>
struct serialization_traits<R>
{
    using size_type = std::ranges::range_size_t<R>;
};

template<typename T>
struct serializer
{
    void operator()(ostream_view& out, const T& data) const = delete;
};

template<typename T>
struct deserializer
{
    void operator()(istream_view& in, T& data) const = delete;
};

} // namespace flow
