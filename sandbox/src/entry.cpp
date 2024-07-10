#include <flow/core/application.hpp>

#include <memory>

// #include "tests/dense_tree_test.hpp"
// #include "tests/input_system_test.hpp"
// #include "tests/textured_quads_test.hpp"
// #include "tests/utility_test.hpp"
// #include "tests/physics_test.hpp"
#include "tests/renderer_line_test.hpp"

namespace flow {

std::unique_ptr<application> entry(int /*argc*/, char** /*argv*/)
{
    return std::make_unique<renderer_line_test>();
}

} // namespace flow
