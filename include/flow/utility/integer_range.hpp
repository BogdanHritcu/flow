#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <limits>
#include <optional>
#include <type_traits>

#include "numeric.hpp"

namespace flow {

template<std::integral T, bool S, bool E>
struct basic_integer_range
{
    using value_type = T;
    using start_comp = std::conditional_t<S, std::greater_equal<T>, std::greater<T>>;
    using end_comp = std::conditional_t<E, std::less_equal<T>, std::less<T>>;

    T start;
    T end;

    [[nodiscard]] constexpr bool contains(value_type value) const noexcept
    {
        return start_comp{}(value, start) && end_comp{}(value, end);
    }

    template<std::floating_point F>
    [[nodiscard]] constexpr value_type lerp(F t) const noexcept
    {
        return flow::lerp(start, end, t);
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept
    {
        return is_valid() ? (end - !E) - (start + !S) + 1 : 0;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
        return start <= std::numeric_limits<T>::max() - !S
            && end >= std::numeric_limits<T>::min() + !E
            && start + !S <= end - !E;
    }

    [[nodiscard]] constexpr bool is_start_inclusive() const noexcept
    {
        return S;
    }

    [[nodiscard]] constexpr bool is_end_inclusive() const noexcept
    {
        return E;
    }

    [[nodiscard]] constexpr operator bool() const noexcept
    {
        return is_valid();
    }
};

template<std::integral T>
using ii_integer_range = basic_integer_range<T, true, true>;

template<std::integral T>
using ei_integer_range = basic_integer_range<T, false, true>;

template<std::integral T>
using ie_integer_range = basic_integer_range<T, true, false>;

template<std::integral T>
using ee_integer_range = basic_integer_range<T, false, false>;

template<std::integral T>
using integer_range = ii_integer_range<T>;

template<std::integral T, bool ST, bool ET, std::integral U, bool SU, bool EU>
[[nodiscard]] constexpr decltype(auto) integer_range_intersection(basic_integer_range<T, ST, ET> rt,
                                                                  basic_integer_range<U, SU, EU> ru) noexcept
{
    using common_type = std::common_type_t<T, U>;
    using range_type = basic_integer_range<common_type, true, true>;

    const auto intersection = std::make_optional<range_type>(std::max(static_cast<common_type>(rt.start + !ST),
                                                                      static_cast<common_type>(ru.start + !SU)),
                                                             std::min(static_cast<common_type>(rt.end - !ET),
                                                                      static_cast<common_type>(ru.end - !EU)));

    return intersection->is_valid() ? intersection : std::optional<range_type>{};
}

template<std::integral... Ts, bool... Ss, bool... Es>
[[nodiscard]] constexpr decltype(auto) integer_range_intersection(basic_integer_range<Ts, Ss, Es>... rs) noexcept
{
    using common_type = std::common_type_t<Ts...>;
    using range_type = basic_integer_range<common_type, true, true>;

    const auto intersection = std::make_optional<range_type>(std::max({ static_cast<common_type>(rs.start + !Ss)... }),
                                                             std::min({ static_cast<common_type>(rs.end - !Es)... }));

    return intersection->is_valid() ? intersection : std::optional<range_type>{};
}

template<std::integral T, bool ST, bool ET, std::integral U, bool SU, bool EU>
[[nodiscard]] constexpr bool integer_has_range_intersection(basic_integer_range<T, ST, ET> rt,
                                                            basic_integer_range<U, SU, EU> ru) noexcept
{
    using common_type = std::common_type_t<T, U>;

    return std::max(static_cast<common_type>(rt.start + !ST), static_cast<common_type>(ru.start + !SU))
        <= std::min(static_cast<common_type>(rt.end - !ET), static_cast<common_type>(ru.end - !EU));
}

template<std::integral... Ts, bool... Ss, bool... Es>
[[nodiscard]] constexpr bool integer_range_has_intersection(basic_integer_range<Ts, Ss, Es>... rs) noexcept
{
    using common_type = std::common_type_t<Ts...>;

    return std::max({ static_cast<common_type>(rs.start + !Ss)... })
        <= std::min({ static_cast<common_type>(rs.end - !Es)... });
}

} // namespace flow
