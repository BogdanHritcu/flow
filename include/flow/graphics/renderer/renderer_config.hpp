#pragma once

#include <cstddef>

#include "../opengl/vertex_attribute.hpp"

namespace flow {
enum class render_mode
{
    points,
    lines,
    line_loop,
    line_strip,
    triangles,
    triangle_strip,
    triangle_fan
};

template<typename VertexT, typename IndexT>
struct renderer_config
{
    using vertex_type = VertexT;
    using index_type = IndexT;
    using size_type = std::size_t;

    std::span<vertex_type> vertices;
    std::span<gl::attribute_config> attributes;
    std::span<index_type> indices;
    size_type capacity;
    size_type buffer_count;
    render_mode render_mode;
};
} // namespace flow
