#include <flow/core/application.hpp>

#include <memory>

// #include "tests/line_renderer_test.hpp"
// #include "tests/rectangle_renderer_test.hpp"
#include "tests/physics_test.hpp"

namespace flow {
std::unique_ptr<application> entry(int /*argc*/, char** /*argv*/)
{
    return std::make_unique<physics_test>();
}
} // namespace flow
