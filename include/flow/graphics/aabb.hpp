#pragma once

#include <cstddef>
#include <cstdint>

#include <glm/gtx/type_aligned.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace flow {

constexpr glm::qualifier DEFAULT_AABB_QUALIFIER = glm::qualifier::packed_highp;

template<std::size_t N, typename T, glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
struct taabb
{
    using value_type = T;
    using vec_type = glm::vec<N, T, Q>;

    vec_type position;
    vec_type size;
};

template<typename T, glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using taabb2 = taabb<2, T, Q>;

template<typename T, glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using taabb3 = taabb<3, T, Q>;

// floating point

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using faabb2 = taabb2<float, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using faabb3 = taabb3<float, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using daabb2 = taabb2<double, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using daabb3 = taabb3<double, Q>;

// signed integers

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using iaabb2 = taabb2<int, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using iaabb3 = taabb3<int, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i8aabb2 = taabb2<std::int8_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i8aabb3 = taabb3<std::int8_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i16aabb2 = taabb2<std::int16_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i16aabb3 = taabb3<std::int16_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i32aabb2 = taabb2<std::int32_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i32aabb3 = taabb3<std::int32_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i64aabb2 = taabb2<std::int64_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using i64aabb3 = taabb3<std::int64_t, Q>;

// unsigned integers

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using uaabb2 = taabb2<unsigned int, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using uaabb3 = taabb3<unsigned int, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u8aabb2 = taabb2<std::uint8_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u8aabb3 = taabb3<std::uint8_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u16aabb2 = taabb2<std::uint16_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u16aabb3 = taabb3<std::uint16_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u32aabb2 = taabb2<std::uint32_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u32aabb3 = taabb3<std::uint32_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u64aabb2 = taabb2<std::uint64_t, Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using u64aabb3 = taabb3<std::uint64_t, Q>;

// defaults

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using aabb2 = faabb2<Q>;

template<glm::qualifier Q = DEFAULT_AABB_QUALIFIER>
using aabb3 = faabb3<Q>;

} // namespace flow
