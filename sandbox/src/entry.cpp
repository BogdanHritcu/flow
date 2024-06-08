#include <flow/core/application.hpp>

#include <memory>

#include "tests/dense_tree_test.hpp"
// #include "tests/input_system_test.hpp"
// #include "tests/textured_quads_test.hpp"
// #include "tests/utility_test.hpp"
// #include "tests/verlet_physics_test.hpp"

namespace flow {

std::unique_ptr<application> entry(int /*argc*/, char** /*argv*/)
{
    return std::make_unique<dense_tree_test>();
}

} // namespace flow
