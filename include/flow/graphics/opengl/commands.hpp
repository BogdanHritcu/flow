#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ranges>

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "enum_types.hpp"

namespace flow::gl {

inline void draw_arrays(primitive_type primitive, std::size_t count, std::size_t offset = 0) noexcept
{
    glDrawArrays(static_cast<GLenum>(primitive), static_cast<GLint>(offset), static_cast<GLsizei>(count));
}

inline void draw_elements(primitive_type primitive, type_value type, std::size_t count, std::size_t offset = 0) noexcept
{
    glDrawElements(static_cast<GLenum>(primitive),
                   static_cast<GLsizei>(count),
                   static_cast<GLenum>(type),
                   reinterpret_cast<void*>(static_cast<std::uintptr_t>(offset * sizeof_type_value(type)))); // NOLINT
}

inline void draw_arrays_instanced(std::size_t instance_count, primitive_type primitive, std::size_t count, std::size_t offset = 0) noexcept
{
    glDrawArraysInstanced(static_cast<GLenum>(primitive),
                          static_cast<GLint>(offset),
                          static_cast<GLsizei>(count),
                          static_cast<GLsizei>(instance_count));
}

inline void draw_elements_instanced(std::size_t instance_count, primitive_type primitive, type_value type, std::size_t count, std::size_t offset = 0) noexcept
{
    glDrawElementsInstanced(static_cast<GLenum>(primitive),
                            static_cast<GLsizei>(count),
                            static_cast<GLenum>(type),
                            reinterpret_cast<void*>(static_cast<std::uintptr_t>(offset * sizeof_type_value(type))), // NOLINT
                            static_cast<GLsizei>(instance_count));
}

template<std::ranges::contiguous_range Rc, std::ranges::contiguous_range Ro>
    requires std::same_as<std::ranges::range_value_t<Rc>, GLint>
        && std::same_as<std::ranges::range_value_t<Ro>, GLsizei>
inline void multi_draw_arrays(primitive_type primitive, Rc&& counts, Ro&& offsets) noexcept
{
    glMultiDrawArrays(static_cast<GLenum>(primitive),
                      std::ranges::data(offsets),
                      std::ranges::data(counts),
                      static_cast<GLsizei>(std::min(std::ranges::size(counts), std::ranges::size(offsets))));
}

inline void clear(clear_target_flags flags = clear_target_flags::color) noexcept
{
    glClear(static_cast<GLbitfield>(flags));
}

inline void set_clear_color(const glm::vec4& color) noexcept
{
    glClearColor(static_cast<GLfloat>(color.r),
                 static_cast<GLfloat>(color.g),
                 static_cast<GLfloat>(color.b),
                 static_cast<GLfloat>(color.a));
}

inline void set_viewport(glm::uvec2 start, glm::uvec2 size) noexcept
{
    glViewport(static_cast<GLint>(start.x),
               static_cast<GLint>(start.y),
               static_cast<GLint>(size.x),
               static_cast<GLint>(size.y));
}

inline void set_polygon_mode(polygon_mode mode) noexcept
{
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(mode));
}

inline void set_blend_function(blend_mode src, blend_mode dst) noexcept
{
    glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
}

inline void enable(render_option option) noexcept
{
    glEnable(static_cast<GLenum>(option));
}

inline void disable(render_option option) noexcept
{
    glDisable(static_cast<GLenum>(option));
}

} // namespace flow::gl
