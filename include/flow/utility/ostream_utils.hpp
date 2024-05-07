#pragma once

#include <ostream>
#include <ranges>

#include "concepts.hpp"

namespace flow {

template<concepts::trivially_copyable T>
std::ostream& write(std::ostream& out, const T& s)
{
    return out.write(reinterpret_cast<const char*>(&s), sizeof(s));
}

template<std::ranges::contiguous_range R>
    requires concepts::trivially_copyable<std::ranges::range_value_t<R>>
std::ostream& write(std::ostream& out, R&& r)
{
    return out.write(reinterpret_cast<const char*>(std::ranges::cdata(r)),
                     std::ranges::size(r) * sizeof(std::ranges::range_value_t<R>));
}

} // namespace flow
