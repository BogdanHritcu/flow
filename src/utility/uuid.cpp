#include "../../include/flow/utility/uuid.hpp"

#include "../../include/flow/utility/random.hpp"

namespace flow {

namespace {
    random_generator generator(std::random_device{}());
}

uuid uuid::generate()
{
    return uuid(generator.uniform_sequence<2>());
}

} // namespace flow
