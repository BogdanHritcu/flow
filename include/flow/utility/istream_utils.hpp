#pragma once

#include <istream>
#include <ranges>

#include "concepts.hpp"

namespace flow {

template<flow::concepts::trivially_copyable T>
std::istream& read(std::istream& in, T& s)
{
    return in.read(reinterpret_cast<char*>(&s), sizeof(s));
}

template<std::ranges::contiguous_range R>
    requires concepts::trivially_copyable<std::ranges::range_value_t<R>>
std::istream& read(std::istream& in, R&& r)
{
    return in.read(reinterpret_cast<char*>(std::ranges::data(r)),
                   std::ranges::size(r) * sizeof(std::ranges::range_value_t<R>));
}

} // namespace flow
