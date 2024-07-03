#pragma once

#include <cstddef>
#include <cstdint>

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "../../utility/concepts.hpp"
#include "enum_types.hpp"

namespace flow {
namespace gl {

    inline void draw_arrays(primitive_type primitive, std::size_t count, std::size_t offset) noexcept
    {
        glDrawArrays(static_cast<GLenum>(primitive), static_cast<GLint>(offset), static_cast<GLsizei>(count));
    }

    inline void draw_elements(primitive_type primitive, type_value type, std::size_t count, std::size_t offset) noexcept
    {
        std::uintptr_t byte_offset = offset * sizeof_type_value(type);

        glDrawElements(static_cast<GLenum>(primitive),
                       static_cast<GLsizei>(count),
                       static_cast<GLenum>(type),
                       reinterpret_cast<void*>(byte_offset));
    }

    inline void draw_arrays_instanced(primitive_type primitive, std::size_t count, std::size_t offset, std::size_t instance_count) noexcept
    {
        glDrawArraysInstanced(static_cast<GLenum>(primitive),
                              static_cast<GLint>(offset),
                              static_cast<GLsizei>(count),
                              static_cast<GLsizei>(instance_count));
    }

    inline void draw_elements_instanced(primitive_type primitive, type_value type, std::size_t count, std::size_t offset, std::size_t instance_count) noexcept
    {
        std::uintptr_t byte_offset = offset * sizeof_type_value(type);

        glDrawElementsInstanced(static_cast<GLenum>(primitive),
                                static_cast<GLsizei>(count),
                                static_cast<GLenum>(type),
                                reinterpret_cast<void*>(byte_offset),
                                static_cast<GLsizei>(instance_count));
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

} // namespace gl
} // namespace flow
