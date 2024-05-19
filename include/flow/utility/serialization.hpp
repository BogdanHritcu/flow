#pragma once

#include <ranges>

#include "traits.hpp"

namespace flow {

class istream_view;
class ostream_view;

template<typename T>
struct serialization_traits
{};

template<std::ranges::sized_range R>
struct serialization_traits<R>
{
    using size_type = std::ranges::range_size_t<R>;
};

template<typename T, typename Traits = serialization_traits<T>>
struct serializer
{};

template<typename T, typename Traits = serialization_traits<T>>
struct deserializer
{};

namespace concepts {

    template<typename F, typename T>
    concept serializer = std::invocable<F,
                                        make_lval_ref_t<ostream_view>,
                                        make_const_lval_ref_t<T>>;

    template<typename F, typename T>
    concept limited_serializer = requires { typename T::size_type; }
                              && std::invocable<F,
                                                make_lval_ref_t<ostream_view>,
                                                make_const_lval_ref_t<T>,
                                                typename T::size_type>;

    template<typename F, typename T>
    concept deserializer = std::invocable<F,
                                          make_lval_ref_t<istream_view>,
                                          make_lval_ref_t<T>>;

    template<typename F, typename T>
    concept limited_deserializer = requires { typename T::size_type; }
                                && std::invocable<F,
                                                  make_lval_ref_t<istream_view>,
                                                  make_lval_ref_t<T>,
                                                  typename T::size_type>;

} // namespace concepts

} // namespace flow
