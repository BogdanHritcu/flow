#pragma once

#include <functional>
#include <string>

#include "../../flow/utility/helpers.hpp"

namespace flow {

struct action
{
    std::string group;
    std::string name;

    constexpr bool operator==(const action&) const noexcept = default;
};

} // namespace flow

template<>
struct std::hash<flow::action>
{
    std::size_t operator()(const flow::action& action) const noexcept
    {
        return flow::hash_combine(std::hash<std::string>{}(action.group), action.name);
    }
};
