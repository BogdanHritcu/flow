#pragma once

#include <type_traits>

namespace flow {

template<typename T>
using make_lval_ref_t = std::remove_const_t<std::add_lvalue_reference_t<std::remove_reference_t<T>>>;

template<typename T>
using make_const_lval_ref_t = std::add_const_t<make_lval_ref_t<T>>;

template<typename T, typename U>
struct max_size
{
    using type = std::conditional_t<(sizeof(T) > sizeof(U)), T, U>;
};

template<typename T, typename U>
using max_size_t = typename max_size<T, U>::type;

template<typename T, typename U>
struct min_size
{
    using type = std::conditional_t<(sizeof(T) < sizeof(U)), T, U>;
};

template<typename T, typename U>
using min_size_t = typename min_size<T, U>::type;

template<typename T, typename U>
struct max_alignment
{
    using type = std::conditional_t<(alignof(T) > alignof(U)), T, U>;
};

template<typename T, typename U>
using max_alignment_t = typename max_alignment<T, U>::type;

template<typename T, typename U>
struct min_alignment
{
    using type = std::conditional_t<(alignof(T) < alignof(U)), T, U>;
};

template<typename T, typename U>
using min_alignment_t = typename min_alignment<T, U>::type;

} // namespace flow
