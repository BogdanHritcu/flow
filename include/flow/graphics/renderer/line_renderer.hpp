#pragma once

#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_aligned.hpp>

#include "../../core/logger.hpp"
#include "../../utility/concepts.hpp"
#include "../opengl/buffer.hpp"
#include "../opengl/commands.hpp"
#include "../opengl/enum_types.hpp"
#include "../opengl/fence.hpp"
#include "../opengl/shader.hpp"
#include "../opengl/vertex_array.hpp"

namespace flow {
class line_renderer
{
private:
    static constexpr std::string_view vertex_shader_source = R"(
    /*
    *   vertex shader for line drawing based on the stackoverflow answer:
    *   https://stackoverflow.com/questions/60440682/drawing-a-line-in-modern-opengl
    */

    #version 460

    struct vertex
    {
        vec4 color;
        vec4 position;
    };

    layout(std430, binding = 0) readonly buffer vertex_buffer
    {
        vertex vertices[];
    };

    layout(location = 0) uniform mat4 u_mvp;
    layout(location = 1) uniform vec2 u_resolution;
    layout(location = 2) uniform float u_line_width;

    out vec4 v_color;

    void main()
    {
        uint line_index = gl_VertexID / 6;
        uint triangle_index = gl_VertexID % 6;

        uint a = (triangle_index & 0x4) >> 2;
        uint b = (triangle_index & 0x2) >> 1;
        uint c = triangle_index & 0x1;

        uint is_end_vertex = a | (b & (~c & 0x1));

        vec4 line_vertices[4];
        for (int i = 0; i < 4; ++i)
        {
            line_vertices[i] = u_mvp * vertices[line_index + i].position;
            line_vertices[i].xyz /= line_vertices[i].w;
            line_vertices[i].xy = (line_vertices[i].xy + 1.0) * 0.5 * u_resolution;
        }

        vec2 line = line_vertices[2].xy - line_vertices[1].xy;
        vec2 line_normal = normalize(vec2(-line.y, line.x));

        uint other_line_index = is_end_vertex << 1;

        vec2 other_line = line_vertices[other_line_index + 1].xy - line_vertices[other_line_index].xy;
        vec2 miter = normalize(line_normal + normalize(vec2(-other_line.y, other_line.x)));
        vec4 position = line_vertices[is_end_vertex + 1];

        float line_width_offset = float(((~a & 0x1) & (~b & 0x1) & (~c & 0x1)) | ((a | b) & c)) - 0.5;

        position.xy += miter * (u_line_width * line_width_offset / dot(miter, line_normal));

        position.xy = position.xy / u_resolution * 2.0 - 1.0;
        position.xyz *= position.w;
        gl_Position = position;

        v_color = vertices[line_index + 1 + is_end_vertex].color;
    }
    )";

    static constexpr std::string_view fragment_shader_source = R"(
    #version 460

    in vec4 v_color;
    out vec4 out_color;

    void main()
    {
        out_color = v_color;
    }
    )";

    struct internal_vertex
    {
        glm::aligned_vec4 color;
        glm::aligned_vec4 position;
    };

public:
    struct vertex
    {
        glm::vec4 color;
        glm::vec2 position;
    };

public:
    bool create(std::size_t line_capacity, std::size_t buffer_count = 3)
    {
        gl::shader vertex_shader;
        gl::shader fragment_shader;

        if (!(vertex_shader.create(flow::gl::shader_type::vertex)
            && vertex_shader.from_string(vertex_shader_source)
            && vertex_shader.compile()))
        {
            FLOW_LOG_ERROR("failed to create vertex shader: {}", vertex_shader.get_info_log());
            return false;
        }

        if (!(fragment_shader.create(flow::gl::shader_type::fragment)
            && fragment_shader.from_string(fragment_shader_source)
            && fragment_shader.compile()))
        {
            FLOW_LOG_ERROR("failed to create fragment shader: {}", fragment_shader.get_info_log());
            return false;
        }

        if (!(m_renderer.shader.create()
            && m_renderer.shader.link(vertex_shader, fragment_shader)))
        {
            FLOW_LOG_ERROR("failed to link shaders: {}", m_renderer.shader.get_info_log());
            return false;
        }

        if (!m_renderer.vao.create())
        {
            FLOW_LOG_ERROR("failed to create vertex array");
            return false;
        }

        if (!m_renderer.ssbo.create())
        {
            FLOW_LOG_ERROR("failed to create shader storage");
            return false;
        }

        std::size_t vertex_capacity = line_capacity * 2;
        m_renderer.buffers.resize(buffer_count);

        using sf = flow::gl::buffer_storage_flags;
        using mf = flow::gl::buffer_map_flags;

        std::size_t ssbo_size = vertex_capacity * m_renderer.buffers.size();
        auto ssbo_storage_flags = sf::dynamic_storage | sf::map_write | sf::map_persistent | sf::map_coherent;
        m_renderer.ssbo.storage(ssbo_size, ssbo_storage_flags);

        auto ssbo_map_flags = mf::write | mf::persistent | mf::coherent;
        auto ssbo_span = m_renderer.ssbo.map(ssbo_size, 0, ssbo_map_flags);

        for (std::size_t i = 0; i < m_renderer.buffers.size(); ++i)
        {
            m_renderer.buffers[i].span = ssbo_span.subspan(i * vertex_capacity,
                                                           vertex_capacity);
        }

        return true;
    }

    void begin_batch(const glm::mat4& mvp, const glm::vec2& resolution, float line_width)
    {
        m_renderer.shader.use();
        m_renderer.shader.set_uniform(0, mvp);
        m_renderer.shader.set_uniform(1, resolution);
        m_renderer.shader.set_uniform(2, line_width);

        reset_current_buffer();
    }

    template<std::ranges::range R>
        requires std::same_as<std::ranges::range_value_t<R>, vertex>
    void submit_strip(R&& vertices, float z)
    {
        auto vertex_count = std::ranges::size(vertices);

        if (vertex_count < 2)
        {
            return;
        }

        auto& current_buffer = m_renderer.buffers[m_renderer.current_buffer_index];

        current_buffer.span[m_renderer.vertex_count].position = { compute_position_before_first(vertices[0].position,
                                                                      vertices[1].position),
                                                                  z,
                                                                  1.0f };
        current_buffer.span[m_renderer.vertex_count + vertex_count + 1].position = {
            compute_position_after_last(vertices[vertex_count - 2].position,
                                        vertices[vertex_count - 1].position),
            z,
            1.0f };
        submit_strip_vertices(vertices, z, false);
    }

    template<std::ranges::range R>
    void submit_strip(R&& vertices)
    {
        submit_strip(std::forward<R>(vertices));
    }

    template<std::ranges::range R>
        requires std::same_as<std::ranges::range_value_t<R>, vertex>
    void submit_loop(R&& vertices, float z)
    {
        auto vertex_count = std::ranges::size(vertices);

        if (vertex_count < 2)
        {
            return;
        }

        auto& current_buffer = m_renderer.buffers[m_renderer.current_buffer_index];

        current_buffer.span[m_renderer.vertex_count].position = { vertices[vertex_count - 1].position, z, 1.0f };
        current_buffer.span[m_renderer.vertex_count + vertex_count + 1] = internal_vertex{ .color = vertices[0].color,
            .position = { vertices[0].position, z, 1.0f } };
        current_buffer.span[m_renderer.vertex_count + vertex_count + 2].position = { vertices[1].position, z, 1.0f };

        submit_strip_vertices(vertices, z, true);
    }

    template<std::ranges::range R>
    void submit_loop(R&& vertices)
    {
        submit_loop(std::forward<R>(vertices), 1.0f);
    }

    template<std::ranges::range R>
        requires concepts::any_of<std::ranges::range_value_t<R>, glm::vec2, glm::aligned_vec2>
    void submit_loop(R&& positions, float z, const glm::vec4& color)
    {
        auto make_vertex = [color](auto position) -> vertex
        {
            return { .color = color, .position = position };
        };

        submit_loop(positions | std::views::transform(make_vertex), z);
    }

    template<std::ranges::range R>
    void submit_loop(R&& positions, float z)
    {
        submit_loop(std::forward<R>(positions), z, { 1.0f, 1.0f, 1.0f, 1.0f });
    }

    template<std::ranges::range R>
    void submit_loop(R&& positions, const glm::vec4& color)
    {
        submit_loop(std::forward<R>(positions), 0.0f, color);
    }

    void submit_line(glm::vec2 a, glm::vec2 b, float z, const glm::vec4& color_a, const glm::vec4& color_b)
    {
        auto& current_buffer = m_renderer.buffers[m_renderer.current_buffer_index];
        auto buffer_capacity = current_buffer.span.size();

        if (m_renderer.vertex_count + 4 > buffer_capacity)
        {
            end_batch();
            reset_current_buffer();
        }

        current_buffer.span[m_renderer.vertex_count + 0].position = { compute_position_before_first(a, b), z, 1.0f };
        current_buffer.span[m_renderer.vertex_count + 1] = { .color = color_a, .position = { a, z, 1.0f } };
        current_buffer.span[m_renderer.vertex_count + 2] = { .color = color_b, .position = { b, z, 1.0f } };
        current_buffer.span[m_renderer.vertex_count + 3].position = { compute_position_after_last(a, b), z, 1.0f };

        constexpr auto draw_vertices_per_line = 6;
        m_renderer.draw_vertex_counts.emplace_back(draw_vertices_per_line);
        m_renderer.draw_vertex_starts.emplace_back(m_renderer.draw_vertex_start);

        m_renderer.vertex_count += 4;
        m_renderer.draw_vertex_start += 4 * draw_vertices_per_line;
    }

    void submit_line(const glm::vec2& a, const glm::vec2& b, float z)
    {
        submit_line(a, b, z, { 1.0f, 1.0f, 1.0f, 1.0f });
    }

    void submit_line(const glm::vec2& a, const glm::vec2& b, const glm::vec4& color_a, const glm::vec4& color_b)
    {
        submit_line(a, b, 0.0f, color_a, color_b);
    }

    void submit_line(const glm::vec2& a, const glm::vec2& b, float z, const glm::vec4& color)
    {
        submit_line(a, b, z, color, color);
    }

    void submit_line(const glm::vec2& a, const glm::vec2& b, const glm::vec4& color)
    {
        submit_line(a, b, 0.0f, color);
    }

    void submit_line(const glm::vec2& a, const glm::vec2& b)
    {
        submit_line(a, b, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
    }

    void end_batch(bool reset = true)
    {
        if (m_renderer.vertex_count >= 4)
        {
            m_renderer.vao.bind();

            auto buffer_capacity = m_renderer.buffers[m_renderer.current_buffer_index].span.size();

            m_renderer.ssbo.bind_range(gl::buffer_target::shader_storage,
                                       0,
                                       m_renderer.vertex_count,
                                       static_cast<std::ptrdiff_t>(m_renderer.current_buffer_index * buffer_capacity));

            gl::multi_draw_arrays(gl::primitive_type::triangles,
                                  m_renderer.draw_vertex_counts,
                                  m_renderer.draw_vertex_starts);

            m_renderer.buffers[m_renderer.current_buffer_index].fence.lock();
            m_renderer.current_buffer_index = (m_renderer.current_buffer_index + reset) % m_renderer.buffers.size();
        }
    }

private:
    template<std::ranges::range R>
        requires std::same_as<std::ranges::range_value_t<R>, vertex>
    void submit_strip_vertices(R&& vertices, float z, bool is_loop)
    {
        auto vertex_count = std::ranges::size(vertices);

        auto& current_buffer = m_renderer.buffers[m_renderer.current_buffer_index];
        auto buffer_capacity = current_buffer.span.size();

        if (m_renderer.vertex_count + vertex_count + 2 > buffer_capacity)
        {
            end_batch();
            reset_current_buffer();
        }

        for (std::size_t i = 0; i < vertex_count; ++i)
        {
            current_buffer.span[m_renderer.vertex_count + i + 1] = internal_vertex{
                .color = vertices[i].color,
                .position = { vertices[i].position, z, 1.0f } };
        }

        vertex_count += is_loop;
        constexpr auto draw_vertices_per_line = 6;
        m_renderer.draw_vertex_counts.emplace_back(draw_vertices_per_line * (vertex_count - 1));
        m_renderer.draw_vertex_starts.emplace_back(m_renderer.draw_vertex_start);

        m_renderer.vertex_count += vertex_count + 2;
        m_renderer.draw_vertex_start += static_cast<GLint>((vertex_count + 2) * draw_vertices_per_line);
    }

    void reset_current_buffer()
    {
        m_renderer.vertex_count = 0;
        m_renderer.draw_vertex_start = 0;
        m_renderer.draw_vertex_counts.resize(0);
        m_renderer.draw_vertex_starts.resize(0);
        m_renderer.buffers[m_renderer.current_buffer_index].fence.wait();
    }

    [[nodiscard]] static constexpr glm::vec2 compute_position_before_first(
            const glm::vec2& a,
            const glm::vec2& b) noexcept
    {
        return a - (b - a);
    }

    [[nodiscard]] static constexpr glm::vec2 compute_position_after_last(
            const glm::vec2& a,
            const glm::vec2& b) noexcept
    {
        return b + (b - a);
    }

private:
    template<typename T>
    struct renderer_state
    {
        using size_type = typename gl::buffer<T>::size_type;
        using offset_type = typename gl::buffer<T>::offset_type;

        struct fenced_buffer
        {
            std::span<T> span;
            gl::fence fence;
        };

        gl::vertex_array vao;
        gl::buffer<T> ssbo;
        gl::shader_program shader;
        std::vector<fenced_buffer> buffers;
        size_type current_buffer_index{};
        size_type vertex_count{};
        GLint draw_vertex_start{};
        std::vector<GLsizei> draw_vertex_counts;
        std::vector<GLint> draw_vertex_starts;
    };

    renderer_state<internal_vertex> m_renderer{};
};
} // namespace flow
