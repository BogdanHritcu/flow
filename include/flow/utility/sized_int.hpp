#pragma once

#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace flow {

// clang-format off

template<std::size_t Width> struct sized_int {};
template<> struct sized_int<8>  { using type = std::int8_t; };
template<> struct sized_int<16> { using type = std::int16_t; };
template<> struct sized_int<32> { using type = std::int32_t; };
template<> struct sized_int<64> { using type = std::int64_t; };

template<std::size_t Width> struct sized_uint {};
template<> struct sized_uint<8>  { using type = std::uint8_t; };
template<> struct sized_uint<16> { using type = std::uint16_t; };
template<> struct sized_uint<32> { using type = std::uint32_t; };
template<> struct sized_uint<64> { using type = std::uint64_t; };

// clang-format on

template<std::size_t Width>
using sized_int_t = typename sized_int<Width>::type;

template<std::size_t Width>
using sized_uint_t = typename sized_uint<Width>::type;

template<std::integral T>
struct make_doubled_width_integral
{
    using type = std::conditional_t<std::is_signed_v<T>,
                                    sized_int_t<2 * sizeof(T) * CHAR_BIT>,
                                    sized_uint_t<2 * sizeof(T) * CHAR_BIT>>;
};

template<std::integral T>
using make_doubled_width_integral_t = typename make_doubled_width_integral<T>::type;

namespace concepts {

    template<typename T>
    concept doubled_width_integral = std::integral<T>
                                  && requires { typename make_doubled_width_integral<T>::type; };

} // namespace concepts

} // namespace flow
