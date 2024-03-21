#pragma once

#include <glm/vec2.hpp>

namespace flow {

template<std::size_t L, typename T>
struct basic_bounds
{
    using vec_type = glm::vec<L, T>;

    vec_type position;
    vec_type size;
};

template<typename T>
struct basic_bounds<2, T>
{
    using value_type = T;
    using vec_type = glm::vec<2, value_type>;

    vec_type position;
    vec_type size;
};

template<typename T>
using rect_bounds = basic_bounds<2, T>;

enum class rect_origin
{
    tl,
    bl,
    br,
    tr
};

template<typename T, rect_origin Origin = rect_origin::tl>
[[nodiscard]] constexpr rect_bounds<T>::vec_type top_left(const rect_bounds<T>& bounds) noexcept
{
    using vec_type = rect_bounds<T>::vec_type;

    // clang-format off

         if constexpr (Origin == rect_origin::tl) return vec_type{ bounds.position.x, bounds.position.y };
    else if constexpr (Origin == rect_origin::bl) return vec_type{ bounds.position.x, bounds.position.y + bounds.size.y };
    else if constexpr (Origin == rect_origin::br) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y };
    else if constexpr (Origin == rect_origin::tr) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y };

    // clang-format on
}

template<typename T, rect_origin Origin = rect_origin::tl>
[[nodiscard]] constexpr rect_bounds<T>::vec_type bottom_left(const rect_bounds<T>& bounds) noexcept
{
    using vec_type = rect_bounds<T>::vec_type;

    // clang-format off

         if constexpr (Origin == rect_origin::tl) return vec_type{ bounds.position.x, bounds.position.y + bounds.size.y };
    else if constexpr (Origin == rect_origin::bl) return vec_type{ bounds.position.x, bounds.position.y };
    else if constexpr (Origin == rect_origin::br) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y };
    else if constexpr (Origin == rect_origin::tr) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y };

    // clang-format on
}

template<typename T, rect_origin Origin = rect_origin::tl>
[[nodiscard]] constexpr rect_bounds<T>::vec_type bottom_right(const rect_bounds<T>& bounds) noexcept
{
    using vec_type = rect_bounds<T>::vec_type;

    // clang-format off

         if constexpr (Origin == rect_origin::tl) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y };
    else if constexpr (Origin == rect_origin::bl) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y };
    else if constexpr (Origin == rect_origin::br) return vec_type{ bounds.position.x, bounds.position.y };
    else if constexpr (Origin == rect_origin::tr) return vec_type{ bounds.position.x, bounds.position.y + bounds.size.y };

    // clang-format on
}

template<typename T, rect_origin Origin = rect_origin::tl>
[[nodiscard]] constexpr rect_bounds<T>::vec_type top_right(const rect_bounds<T>& bounds) noexcept
{
    using vec_type = rect_bounds<T>::vec_type;

    // clang-format off

         if constexpr (Origin == rect_origin::tl) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y };
    else if constexpr (Origin == rect_origin::bl) return vec_type{ bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y };
    else if constexpr (Origin == rect_origin::br) return vec_type{ bounds.position.x, bounds.position.y + bounds.size.y };
    else if constexpr (Origin == rect_origin::tr) return vec_type{ bounds.position.x, bounds.position.y };

    // clang-format on
}

} // namespace flow
