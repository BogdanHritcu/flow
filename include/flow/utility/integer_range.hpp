#pragma once

#include <algorithm>
#include <concepts>
#include <optional>
#include <type_traits>

namespace flow {

template<std::integral T, bool S, bool E>
struct basic_integer_range
{
    using value_type = T;

    T start;
    T end;

    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
        return start + !S <= end - !E;
    }

    [[nodiscard]] constexpr bool is_start_inclusive() const noexcept
    {
        return S;
    }

    [[nodiscard]] constexpr bool is_end_inclusive() const noexcept
    {
        return E;
    }
};

template<std::integral T>
using basic_ii_integer_range = basic_integer_range<T, true, true>;

template<std::integral T>
using basic_ei_integer_range = basic_integer_range<T, false, true>;

template<std::integral T>
using basic_ie_integer_range = basic_integer_range<T, true, false>;

template<std::integral T>
using basic_ee_integer_range = basic_integer_range<T, false, false>;

using ii_integer_range = basic_ii_integer_range<std::int32_t>;
using ei_integer_range = basic_ei_integer_range<std::int32_t>;
using ie_integer_range = basic_ie_integer_range<std::int32_t>;
using ee_integer_range = basic_ee_integer_range<std::int32_t>;
using integer_range = ii_integer_range;

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
