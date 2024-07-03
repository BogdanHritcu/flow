#pragma once

#include <cstddef>

#include <glad/gl.h>

#include "../../utility/concepts.hpp"
#include "../renderer/renderer_config.hpp"

namespace flow {
namespace gl {

    enum class type_value : GLenum
    {
        // clang-format off

        gl_float  = GL_FLOAT,
        gl_double = GL_DOUBLE,
        gl_byte   = GL_BYTE,
        gl_short  = GL_SHORT,
        gl_int    = GL_INT,
        gl_ubyte  = GL_UNSIGNED_BYTE,
        gl_ushort = GL_UNSIGNED_SHORT,
        gl_uint   = GL_UNSIGNED_INT

        // clang-format on
    };

    enum class primitive_type : GLenum
    {
        // clang-format off

        points         = GL_POINTS,
        lines          = GL_LINES,
        line_strip     = GL_LINE_STRIP,
        line_loop      = GL_LINE_LOOP,
        triangles      = GL_TRIANGLES,
        triangle_strip = GL_TRIANGLE_STRIP,
        triangle_fan   = GL_TRIANGLE_FAN

        // clang-format on
    };

    enum class polygon_mode : GLenum
    {
        // clang-format off

        point = GL_POINT,
        line  = GL_LINE,
        fill  = GL_FILL

        // clang-format on
    };

    enum class clear_target_flags : GLbitfield
    {
        // clang-format off

        color   = GL_COLOR_BUFFER_BIT,
        depth   = GL_DEPTH_BUFFER_BIT,
        stencil = GL_STENCIL_BUFFER_BIT

        // clang-format on
    };

    [[nodiscard]] constexpr clear_target_flags operator|(clear_target_flags lhs, clear_target_flags rhs) noexcept
    {
        return static_cast<clear_target_flags>(static_cast<GLbitfield>(lhs) | static_cast<GLbitfield>(rhs));
    }

    template<concepts::any_of<GLfloat,
                              GLdouble,
                              GLbyte,
                              GLshort,
                              GLint,
                              GLubyte,
                              GLushort,
                              GLuint> Type>
    [[nodiscard]] constexpr type_value to_type_value() noexcept
    {
        // clang-format off

             if constexpr (std::same_as<Type, GLfloat>)  return type_value::gl_float;
        else if constexpr (std::same_as<Type, GLdouble>) return type_value::gl_double;
        else if constexpr (std::same_as<Type, GLbyte>)   return type_value::gl_byte;
        else if constexpr (std::same_as<Type, GLshort>)  return type_value::gl_short;
        else if constexpr (std::same_as<Type, GLint>)    return type_value::gl_int;
        else if constexpr (std::same_as<Type, GLubyte>)  return type_value::gl_ubyte;
        else if constexpr (std::same_as<Type, GLushort>) return type_value::gl_ushort;
        else if constexpr (std::same_as<Type, GLuint>)   return type_value::gl_uint;

        // clang-format on
    }

    [[nodiscard]] constexpr gl::primitive_type to_primitive_type(render_mode mode) noexcept
    {
        switch (mode)
        {
            // clang-format off

        case render_mode::points:         return gl::primitive_type::points;
        case render_mode::lines:          return gl::primitive_type::lines;
        case render_mode::line_loop:      return gl::primitive_type::line_loop;
        case render_mode::line_strip:     return gl::primitive_type::line_strip;
        case render_mode::triangles:      return gl::primitive_type::triangles;
        case render_mode::triangle_strip: return gl::primitive_type::triangle_strip;
        case render_mode::triangle_fan:   return gl::primitive_type::triangle_fan;
        default:                          return gl::primitive_type::triangles;

            // clang-format on
        }
    }

    [[nodiscard]] constexpr std::size_t sizeof_type_value(type_value type) noexcept
    {
        switch (type)
        {
            // clang-format off

        case flow::gl::type_value::gl_float : return sizeof(GLfloat);
        case flow::gl::type_value::gl_double: return sizeof(GLdouble);
        case flow::gl::type_value::gl_byte  : return sizeof(GLbyte);
        case flow::gl::type_value::gl_short : return sizeof(GLshort);
        case flow::gl::type_value::gl_int   : return sizeof(GLint);
        case flow::gl::type_value::gl_ubyte : return sizeof(GLubyte);
        case flow::gl::type_value::gl_ushort: return sizeof(GLushort);
        case flow::gl::type_value::gl_uint  : return sizeof(GLuint);
        default                             : return 1;

            // clang-format on
        }
    }

} // namespace gl
} // namespace flow
