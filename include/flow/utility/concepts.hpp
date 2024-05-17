#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

namespace flow::concepts {

template<typename T, typename... Args>
concept nothrow_default_constructible = std::is_nothrow_default_constructible_v<T>;

template<typename T, typename... Args>
concept nothrow_constructible = std::is_nothrow_constructible_v<T, Args...>;

template<typename T>
concept trivially_copyable = std::is_trivially_copyable_v<T>;

template<typename T>
concept trivially_copyable_range = std::ranges::contiguous_range<T>
                                && trivially_copyable<std::ranges::range_value_t<T>>;

template<typename T>
concept resizable_range = std::ranges::sized_range<T>
                       && requires(T& t) { t.resize(std::ranges::size(t)); };

template<typename T, typename... Args>
concept nothrow_invocable = std::is_nothrow_invocable_v<T, Args...>;

template<typename T, typename R, typename... Args>
concept invocable_r = std::is_invocable_r_v<T, R, Args...>;

template<typename T>
concept member_function = std::is_member_function_pointer_v<T>;

template<typename T, typename... Args>
concept nothrow_operator_callable = member_function<decltype(&T::operator())> && nothrow_invocable<decltype(&T::operator()), T>;

template<typename T>
concept nothrow_hashable = nothrow_constructible<std::hash<T>> && nothrow_invocable<decltype(&std::hash<T>::operator()), std::hash<T>, T>;

template<typename T>
concept boolean = std::same_as<T, bool>;

template<typename T>
concept single_precision_float = std::same_as<T, float> && (sizeof(T) == 4);

template<typename T>
concept double_precision_float = std::same_as<T, double> && (sizeof(T) == 8);

template<typename T>
concept arithmetic = std::integral<T> || single_precision_float<T> || double_precision_float<T>;

template<typename T>
concept non_boolean_arithmetic = arithmetic<T> && !boolean<T>;

template<typename T, typename... Args>
concept any_of = (... || std::same_as<T, Args>);

template<auto Value, decltype(Value)... Args>
concept any_value_of = std::equality_comparable<decltype(Value)> && (... || (Value == Args));

template<std::size_t N, typename T, typename... Ts>
concept in_pack_n = (... + static_cast<std::size_t>(std::same_as<T, Ts>)) == N;

template<std::size_t N, typename T, typename... Ts>
concept in_pack_min_n = (... + static_cast<std::size_t>(std::same_as<T, Ts>)) >= N;

template<std::size_t N, typename T, typename... Ts>
concept in_pack_max_n = (... + static_cast<std::size_t>(std::same_as<T, Ts>)) <= N;

template<typename T, typename... Ts>
concept in_pack = in_pack_min_n<1, T, Ts...>;

template<std::size_t N, typename... Ts>
concept all_in_pack_n = (... && in_pack_n<N, Ts, Ts...>);

template<std::size_t N, typename... Ts>
concept all_in_pack_min_n = (... && in_pack_min_n<N, Ts, Ts...>);

template<std::size_t N, typename... Ts>
concept all_in_pack_max_n = (... && in_pack_max_n<N, Ts, Ts...>);

template<typename T, typename U, typename... Vs>
concept exclusive_in_pack = (in_pack<T, Vs...> && !in_pack<U, Vs...>)
                         || (!in_pack<T, Vs...> && in_pack<U, Vs...>)
                         || (!in_pack<T, Vs...> && !in_pack<U, Vs...>);

} // namespace flow::concepts
