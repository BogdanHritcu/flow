#pragma once

#include <type_traits>

namespace flow {

template<typename T>
using make_lval_ref_t = std::remove_const_t<std::add_lvalue_reference_t<std::remove_reference_t<T>>>;

template<typename T>
using make_const_lval_ref_t = std::add_const_t<make_lval_ref_t<T>>;

} // namespace flow
