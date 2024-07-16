#pragma once

#include <ranges>
#include <string_view>
#include <utility>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_aligned.hpp>

#include "../../core/logger.hpp"
#include "../opengl/buffer.hpp"
#include "../opengl/commands.hpp"
#include "../opengl/enum_types.hpp"
#include "../opengl/fence.hpp"
#include "../opengl/shader.hpp"
#include "../opengl/vertex_array.hpp"

namespace flow {
class rectangle_renderer
{
private:
    static constexpr std::string_view vertex_shader_source = R"(
    #version 460

    vec4 default_vertices[6] =
    {
        {  0.0, 1.0, 0.0, 1.0 },
        {  0.0, 0.0, 0.0, 1.0 },
        {  1.0, 1.0, 0.0, 1.0 },
        {  0.0, 0.0, 0.0, 1.0 },
        {  1.0, 0.0, 0.0, 1.0 },
        {  1.0, 1.0, 0.0, 1.0 }
    };

    struct instance
    {
        mat4 transform;
        vec4 color;
    };

    layout(std430, binding = 0) readonly buffer instance_buffer
    {
        instance instances[];
    };

    layout(location = 0) uniform mat4 u_view_proj;

    out vec4 v_color;

    void main()
    {
        uint instance_index = gl_VertexID / 6;
        uint relative_vertex_index = gl_VertexID % 6;

        v_color = instances[instance_index].color;
        gl_Position = u_view_proj * instances[instance_index].transform * default_vertices[relative_vertex_index];
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

    struct instance
    {
        glm::aligned_mat4 transform;
        glm::aligned_vec4 color;
    };

public:
    bool create(std::size_t capacity, std::size_t buffer_count = 3)
    {
        gl::shader vertex_shader;
        gl::shader fragment_shader;

        if (!(vertex_shader.create(gl::shader_type::vertex)
            && vertex_shader.from_string(vertex_shader_source)
            && vertex_shader.compile()))
        {
            FLOW_LOG_ERROR("failed to create vertex shader: {}", vertex_shader.get_info_log());
            return false;
        }

        if (!(fragment_shader.create(gl::shader_type::fragment)
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

        m_renderer.buffers.resize(buffer_count);

        using sf = gl::buffer_storage_flags;
        using mf = gl::buffer_map_flags;

        std::size_t ssbo_size = capacity * m_renderer.buffers.size();
        auto ssbo_storage_flags = sf::dynamic_storage | sf::map_write | sf::map_persistent | sf::map_coherent;
        m_renderer.ssbo.storage(ssbo_size, ssbo_storage_flags);

        auto ssbo_map_flags = mf::write | mf::persistent | mf::coherent;
        auto ssbo_span = m_renderer.ssbo.map(ssbo_size, 0, ssbo_map_flags);

        for (std::size_t i = 0; i < m_renderer.buffers.size(); ++i)
        {
            m_renderer.buffers[i].span = ssbo_span.subspan(i * capacity,
                                                           capacity);
        }

        return true;
    }

    void begin_batch(const glm::mat4& view_proj = glm::mat4(1.0f)) noexcept
    {
        set_view_projection(view_proj);
        reset_current_buffer();
    }

    void end_batch(bool reset = true) noexcept
    {
        if (m_renderer.instance_count > 0)
        {
            m_renderer.vao.bind();

            auto buffer_capacity = m_renderer.buffers[m_renderer.current_buffer_index].span.size();

            m_renderer.ssbo.bind_range(gl::buffer_target::shader_storage,
                                       0,
                                       m_renderer.instance_count,
                                       static_cast<std::ptrdiff_t>(m_renderer.current_buffer_index * buffer_capacity));

            m_renderer.shader.use();
            m_renderer.shader.set_uniform(0, m_renderer.view_proj);

            gl::draw_arrays(gl::primitive_type::triangles, m_renderer.instance_count * 6); // NOLINT(*-avoid-magic-numbers)

            m_renderer.buffers[m_renderer.current_buffer_index].fence.lock();
            m_renderer.current_buffer_index = (m_renderer.current_buffer_index + reset) % m_renderer.buffers.size();
        }
    }

    void submit(const glm::mat4& transform,
                const glm::vec4& color)
    {
        auto& current_buffer = m_renderer.buffers[m_renderer.current_buffer_index];
        auto buffer_capacity = current_buffer.span.size();

        if (m_renderer.instance_count >= buffer_capacity)
        {
            end_batch();
            reset_current_buffer();
        }

        current_buffer.span[m_renderer.instance_count] = { .transform = transform, .color = color };

        ++m_renderer.instance_count;
    }

    void submit(const glm::vec3& position,
                const glm::vec2& size,
                float angle,
                const glm::vec2& origin,
                const glm::vec4& color)
    {
        glm::mat4 transform{ 1.0f };
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, glm::radians(angle), { 0.0f, 0.0f, 1.0f });
        transform = glm::scale(transform, { size, 1.0f });
        transform = glm::translate(transform, { -origin.x, -origin.y, 0.0f });

        submit(transform, color);
    }

    void submit(const glm::vec2& position,
                const glm::vec2& size,
                float angle,
                const glm::vec2& origin,
                const glm::vec4& color)
    {
        submit(glm::vec3(position, 0.0f), size, angle, origin, color);
    }

    void submit(const glm::vec3& position,
                const glm::vec2& size,
                float angle,
                const glm::vec4& color)
    {
        glm::mat4 transform{ 1.0f };
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, glm::radians(angle), { 0.0f, 0.0f, 1.0f });
        transform = glm::scale(transform, { size, 1.0f });

        submit(transform, color);
    }

    void submit(const glm::vec2& position,
                const glm::vec2& size,
                float angle,
                const glm::vec4& color)
    {
        submit(glm::vec3(position, 0.0f), size, angle, color);
    }

    void submit(const glm::vec3& position,
                const glm::vec2& size,
                const glm::vec2& origin,
                const glm::vec4& color)
    {
        glm::mat4 transform{ 1.0f };
        transform = glm::translate(transform, position);
        transform = glm::scale(transform, { size, 1.0f });
        transform = glm::translate(transform, { -origin.x, -origin.y, 0.0f });

        submit(transform, color);
    }

    void submit(const glm::vec2& position,
                const glm::vec2& size,
                const glm::vec2& origin,
                const glm::vec4& color)
    {
        submit(glm::vec3(position, 0.0f), size, origin, color);
    }

    void submit(const glm::vec3& position,
                const glm::vec2& size,
                const glm::vec4& color)
    {
        glm::mat4 transform{ 1.0f };
        transform = glm::translate(transform, position);
        transform = glm::scale(transform, { size, 1.0f });

        submit(transform, color);
    }

    void submit(const glm::vec2& position,
                const glm::vec2& size,
                const glm::vec4& color)
    {
        submit(glm::vec3(position, 0.0f), size, color);
    }

    void set_view_projection(const glm::mat4& view_proj) noexcept
    {
        m_renderer.view_proj = view_proj;
    }

private:
    void reset_current_buffer() noexcept
    {
        m_renderer.instance_count = 0;
        m_renderer.buffers[m_renderer.current_buffer_index].fence.wait();
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
        size_type instance_count{};
        glm::mat4 view_proj{};
    };

    renderer_state<instance> m_renderer{};
};
} // namespace flow
