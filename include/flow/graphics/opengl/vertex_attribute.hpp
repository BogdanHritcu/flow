#pragma once

#include <cstddef>

#include <glad/gl.h>

namespace flow::gl {
enum class type_value : GLenum;

struct attribute_format
{
    using size_type = std::size_t;
    using offset_type = std::size_t;

    offset_type offset;
    size_type count;
    type_value type;
};

struct attribute_config
{
    using index_type = GLuint;

    attribute_format format;
    index_type index;
    bool normalize;
};
} // namespace flow::gl
