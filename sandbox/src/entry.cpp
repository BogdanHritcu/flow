#include <flow/core/application.hpp>

#include <memory>

#include "tests/batch_renderer/textured_quads_test.hpp"

namespace flow {

std::unique_ptr<application> entry(int argc, char** argv)
{
    return std::make_unique<texture_quad_test>();
}

} // namespace flow
