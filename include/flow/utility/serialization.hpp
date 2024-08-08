#pragma once

#include "traits.hpp"

namespace flow {

class istream_view;
class ostream_view;

template<typename T>
struct serializer
{};

template<typename T>
struct deserializer
{};

namespace concepts {

    template<typename F, typename T>
    concept serializer = std::invocable<F,
                                        make_lval_ref_t<ostream_view>,
                                        make_const_lval_ref_t<T>>;

    template<typename F, typename T>
    concept deserializer = std::invocable<F,
                                          make_lval_ref_t<istream_view>,
                                          make_lval_ref_t<T>>;

} // namespace concepts

} // namespace flow
