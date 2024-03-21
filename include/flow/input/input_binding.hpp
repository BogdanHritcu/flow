#pragma once

#include "../utility/helpers.hpp"
#include "input_code.hpp"

namespace flow {

struct input_binding
{
    input_code code;
    input_modifier mod;
    input_action action;

    constexpr input_binding(input_code in_code, input_modifier in_mod = input_modifier::none, input_action in_action = input_action::press) noexcept
        : code{ in_code }
        , mod{ in_mod }
        , action{ in_action }
    {}

    constexpr bool operator==(const input_binding&) const noexcept = default;
};

} // namespace flow

template<>
struct std::hash<flow::input_binding>
{
    std::size_t operator()(const flow::input_binding& bind) const noexcept
    {
        std::size_t hash = flow::hash_combine(std::hash<flow::input_code>{}(bind.code), bind.mod);
        hash = flow::hash_combine(hash, bind.action);

        return hash;
    }
};
