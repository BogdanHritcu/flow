#pragma once

#include <cstddef>
#include <functional>

#include "concepts.hpp"
#include "istream_view.hpp"
#include "traits.hpp"

namespace flow {

namespace concepts {
    template<typename F, typename ValueT, typename CompValueT>
    concept left_comparator = std::invocable<F,
                                             make_const_lval_ref_t<ValueT>,
                                             make_const_lval_ref_t<CompValueT>>
                           && requires(F comp, ValueT val, CompValueT comp_val) {
                                  { std::invoke(comp, val, comp_val) } -> std::same_as<bool>;
                              };

    template<typename F, typename CompValueT, typename ValueT>
    concept right_comparator = std::invocable<F,
                                              make_const_lval_ref_t<CompValueT>,
                                              make_const_lval_ref_t<ValueT>>
                            && requires(F comp, CompValueT comp_val, ValueT val) {
                                   { std::invoke(comp, comp_val, val) } -> std::same_as<bool>;
                               };
} // namespace concepts

template<typename CompValueT, concepts::trivially_copyable ValueT = CompValueT>
std::size_t stream_lower_bound(istream_view in,
                               istream_view::pos_type start_position,
                               std::size_t begin_index,
                               std::size_t end_index,
                               const CompValueT& comp_value,
                               concepts::left_comparator<ValueT, CompValueT> auto compare)
{
    std::size_t index{};
    std::ptrdiff_t step{};
    auto count = static_cast<std::ptrdiff_t>(end_index) - static_cast<std::ptrdiff_t>(begin_index);

    ValueT value_at_index{};

    while (count > 0)
    {
        if (!in)
        {
            return end_index;
        }

        index = begin_index;
        step = count / 2;
        index += step;

        const auto value_position = start_position + static_cast<istream_view::off_type>(index * sizeof(ValueT));
        in.seek(value_position).read(value_at_index);

        if (compare(value_at_index, comp_value))
        {
            begin_index = ++index;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    return begin_index;
}

template<concepts::trivially_copyable T>
std::size_t stream_lower_bound(istream_view in,
                               istream_view::pos_type start_position,
                               std::size_t begin_index,
                               std::size_t end_index,
                               const T& comp_value)
{
    return stream_lower_bound(in, start_position, begin_index, end_index, comp_value, std::less{});
}

template<typename CompValueT, concepts::trivially_copyable ValueT = CompValueT>
std::size_t stream_upper_bound(istream_view in,
                               istream_view::pos_type start_position,
                               std::size_t begin_index,
                               std::size_t end_index,
                               const CompValueT& comp_value,
                               concepts::right_comparator<CompValueT, ValueT> auto compare)
{
    std::size_t index{};
    std::ptrdiff_t step{};
    auto count = static_cast<std::ptrdiff_t>(end_index) - static_cast<std::ptrdiff_t>(begin_index);

    ValueT value_at_index{};

    while (count > 0)
    {
        if (!in)
        {
            return end_index;
        }

        index = begin_index;
        step = count / 2;
        index += step;

        const auto value_position = start_position + static_cast<istream_view::off_type>(index * sizeof(ValueT));
        in.seek(value_position).read(value_at_index);

        if (!compare(comp_value, value_at_index))
        {
            begin_index = ++index;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    return begin_index;
}

template<concepts::trivially_copyable T>
std::size_t stream_upper_bound(istream_view in,
                               istream_view::pos_type start_position,
                               std::size_t begin_index,
                               std::size_t end_index,
                               const T& comp_value)
{
    return stream_upper_bound(in, start_position, begin_index, end_index, comp_value, std::less{});
}

} // namespace flow
