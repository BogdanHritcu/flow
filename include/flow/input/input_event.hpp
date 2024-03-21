#pragma once

#include <functional>

#include "input_binding.hpp"

namespace flow {

struct input_event
{
    input_binding binding;
    std::uint64_t timestamp;
};

using input_event_callback_type = std::function<void(const input_event&)>;

} // namespace flow
