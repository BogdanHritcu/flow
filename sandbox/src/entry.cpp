#include <flow/core/application.hpp>

#include <memory>

// #include "tests/textured_quads_test.hpp"
#include "tests/input_context_test.hpp"

namespace flow {

std::unique_ptr<application> entry(int /*argc*/, char** /*argv*/)
{
    return std::make_unique<input_context_test>();
}

} // namespace flow
