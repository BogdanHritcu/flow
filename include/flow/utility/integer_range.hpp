#pragma once

#include <algorithm>
#include <concepts>
#include <optional>
#include <type_traits>

namespace flow {

template<std::integral T, bool StartInclusive, bool EndInclusive>
struct basic_integer_range
{
    using value_type = T;
    static constexpr bool is_start_inclusive = StartInclusive;
    static constexpr bool is_end_inclusive = EndInclusive;

    T start;
    T end;
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

template<std::integral T, bool ST, bool ET>
[[nodiscard]] constexpr decltype(auto) integer_range_intersection(const basic_integer_range<T, ST, ET>& rt) noexcept
{
    using range_type = basic_integer_range<T, true, true>;

    if constexpr (ST && ET)
    {
        if (rt.start > rt.end)
        {
            return std::optional<range_type>{};
        }

        return std::make_optional<range_type>(rt.start, rt.end);
    }
    else
    {
        const auto intersection = std::make_optional<range_type>(rt.start + !ST, rt.end - !ET);

        return intersection->start <= intersection->end ? intersection : std::optional<range_type>{};
    }
}

template<std::integral T, bool ST, bool ET, std::integral U, bool SU, bool EU>
[[nodiscard]] constexpr decltype(auto) integer_range_intersection(const basic_integer_range<T, ST, ET>& rt,
                                                                  const basic_integer_range<U, SU, EU>& ru) noexcept
{
    using common_type = std::common_type_t<T, U>;
    using range_type = basic_integer_range<common_type, true, true>;

    const auto intersection = std::make_optional<range_type>(std::max(static_cast<common_type>(rt.start + !ST),
                                                                      static_cast<common_type>(ru.start + !SU)),
                                                             std::min(static_cast<common_type>(rt.end - !ET),
                                                                      static_cast<common_type>(ru.end - !EU)));

    return intersection->start <= intersection->end ? intersection : std::optional<range_type>{};
}

template<std::integral... Ts, bool... STs, bool... ETs>
[[nodiscard]] constexpr decltype(auto) integer_range_intersection(const basic_integer_range<Ts, STs, ETs>&... rts) noexcept
{
    using common_type = std::common_type_t<Ts...>;
    using range_type = basic_integer_range<common_type, true, true>;

    const auto intersection = std::make_optional<range_type>(std::max({ static_cast<common_type>(rts.start + !STs)... }),
                                                             std::min({ static_cast<common_type>(rts.end - !ETs)... }));

    return intersection->start <= intersection->end ? intersection : std::optional<range_type>{};
}

} // namespace flow
