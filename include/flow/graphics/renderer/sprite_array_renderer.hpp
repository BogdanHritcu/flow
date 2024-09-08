#pragma once

#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_aligned.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <ranges>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "../../core/logger.hpp"
#include "../../math/vec2.hpp"
#include "../opengl/buffer.hpp"
#include "../opengl/commands.hpp"
#include "../opengl/enum_types.hpp"
#include "../opengl/fence.hpp"
#include "../opengl/shader.hpp"
#include "../opengl/texture.hpp"
#include "../opengl/vertex_array.hpp"
#include "../texture/texture_array.hpp"

namespace flow {

class sprite_array_renderer
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
        float tex_layer;
    };

    layout(std430, binding = 0) readonly buffer instance_buffer
    {
        instance instances[];
    };

    layout(location = 0) uniform mat4 u_view_proj;

    out vec4 v_color;
    out vec2 v_tex_coords;
    out float v_tex_layer;

    void main()
    {
        uint instance_index = gl_VertexID / 6;
        uint relative_vertex_index = gl_VertexID % 6;

        // TODO: optimize this part
        float is_top = float(relative_vertex_index == 0 || relative_vertex_index == 2 || relative_vertex_index == 5);
        float is_left = float(relative_vertex_index == 0 || relative_vertex_index == 1 || relative_vertex_index == 3);

        v_color = instances[instance_index].color;
        v_tex_coords = vec2(1.0 - is_left, is_top);
        v_tex_layer = instances[instance_index].tex_layer;

        gl_Position = u_view_proj * instances[instance_index].transform * default_vertices[relative_vertex_index];
    }
    )";

    static constexpr std::string_view fragment_shader_source = R"(
    #version 460

    in vec4 v_color;
    in vec2 v_tex_coords;
    in float v_tex_layer;

    out vec4 out_color;

    layout(binding = 0) uniform sampler2DArray u_texture_array;

    void main()
    {
        out_color = texture(u_texture_array, vec3(v_tex_coords, v_tex_layer)) * v_color;
    }
    )";

    struct instance
    {
        glm::aligned_mat4 transform;
        glm::aligned_vec4 color;
        float tex_layer;
    };

    static constexpr glm::vec4 default_color = { 1.0f, 1.0f, 1.0f, 1.0f };

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

        if (!m_renderer.default_texture.create())
        {
            FLOW_LOG_ERROR("failed to create default texture");
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

        static constexpr std::array<std::uint8_t, 4> white_pixel{ 255, 255, 255, 255 };
        m_renderer.default_texture.storage(1, gl::texture_format::rgba8, 1, 1, 1);
        m_renderer.default_texture.sub_image(std::span{ white_pixel },
                                             0,
                                             gl::pixel_format::rgba,
                                             gl::to_type_value<std::uint8_t>(),
                                             1,
                                             1,
                                             1);

        m_renderer.default_texture.set_filter(gl::texture_filter_direction::minifying, gl::texture_filter_mode::nearest);
        m_renderer.default_texture.set_filter(gl::texture_filter_direction::magnifying, gl::texture_filter_mode::nearest);
        m_renderer.default_texture.set_wrap(gl::texture_wrap_direction::s, gl::texture_wrap_mode::clamp_to_edge);
        m_renderer.default_texture.set_wrap(gl::texture_wrap_direction::t, gl::texture_wrap_mode::clamp_to_edge);

        return true;
    }

    void begin_batch(const glm::mat4& view_proj = glm::mat4(1.0f)) noexcept
    {
        begin_batch(m_renderer.default_texture.id(), view_proj);
    }

    void begin_batch(const texture_array& texture_array, const glm::mat4& view_proj = glm::mat4(1.0f)) noexcept
    {
        begin_batch(texture_array.texture_id(), view_proj);
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
            gl::texture2D_array::bind(m_renderer.texture_id);

            gl::draw_arrays(gl::primitive_type::triangles, m_renderer.instance_count * 6); // NOLINT(*-avoid-magic-numbers)

            m_renderer.buffers[m_renderer.current_buffer_index].fence.lock();
            m_renderer.current_buffer_index = (m_renderer.current_buffer_index + reset) % m_renderer.buffers.size();
        }
    }

    void submit(const glm::mat4& transform,
                std::uint32_t tex_index,
                const concepts::vector_least4<float> auto& color = default_color)
    {
        auto& current_buffer = m_renderer.buffers[m_renderer.current_buffer_index];
        auto buffer_capacity = current_buffer.span.size();

        if (m_renderer.instance_count >= buffer_capacity)
        {
            end_batch();
            reset_current_buffer();
        }

        current_buffer.span[m_renderer.instance_count] = {
            .transform = transform,
            .color = { color.x, color.y, color.z, color.w },
            .tex_layer = static_cast<float>(tex_index),
        };

        ++m_renderer.instance_count;
    }

    void submit(const glm::mat4& transform,
                const concepts::vector_least4<float> auto& color)
    {
        submit(transform, 0u, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                float angle,
                const concepts::vector_least2<float> auto& origin,
                std::uint32_t tex_index,
                const concepts::vector_least4<float> auto& color = default_color)
    {
        glm::mat4 transform{ 1.0f };
        if constexpr (concepts::vector_has_z<decltype(position), float>)
        {
            transform = glm::translate(transform, { position.x, position.y, position.z });
        }
        else
        {
            transform = glm::translate(transform, { position.x, position.y, 0.0f });
        }
        transform = glm::rotate(transform, glm::radians(angle), { 0.0f, 0.0f, 1.0f });
        transform = glm::scale(transform, { size.x, size.y, 1.0f });
        transform = glm::translate(transform, { -origin.x, -origin.y, 0.0f });

        submit(transform, tex_index, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                float angle,
                const concepts::vector_least2<float> auto& origin,
                const concepts::vector_least4<float> auto& color)
    {
        submit(position, size, angle, origin, 0u, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                float angle,
                std::uint32_t tex_index,
                const concepts::vector_least4<float> auto& color = default_color)
    {
        glm::mat4 transform{ 1.0f };
        if constexpr (concepts::vector_has_z<decltype(position), float>)
        {
            transform = glm::translate(transform, { position.x, position.y, position.z });
        }
        else
        {
            transform = glm::translate(transform, { position.x, position.y, 0.0f });
        }
        transform = glm::rotate(transform, glm::radians(angle), { 0.0f, 0.0f, 1.0f });
        transform = glm::scale(transform, { size.x, size.y, 1.0f });

        submit(transform, tex_index, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                float angle,
                const concepts::vector_least4<float> auto& color)
    {
        submit(position, size, angle, 0u, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                const concepts::vector_least2<float> auto& origin,
                std::uint32_t tex_index,
                const concepts::vector_least4<float> auto& color = default_color)
    {
        glm::mat4 transform{ 1.0f };
        if constexpr (concepts::vector_has_z<decltype(position), float>)
        {
            transform = glm::translate(transform, { position.x, position.y, position.z });
        }
        else
        {
            transform = glm::translate(transform, { position.x, position.y, 0.0f });
        }
        transform = glm::scale(transform, { size.x, size.y, 1.0f });
        transform = glm::translate(transform, { -origin.x, -origin.y, 0.0f });

        submit(transform, tex_index, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                const concepts::vector_least2<float> auto& origin,
                const concepts::vector_least4<float> auto& color)
    {
        submit(position, size, origin, 0, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                std::uint32_t tex_index,
                const concepts::vector_least4<float> auto& color = default_color)
    {
        glm::mat4 transform{ 1.0f };
        if constexpr (concepts::vector_has_z<decltype(position), float>)
        {
            transform = glm::translate(transform, { position.x, position.y, position.z });
        }
        else
        {
            transform = glm::translate(transform, { position.x, position.y, 0.0f });
        }
        transform = glm::scale(transform, { size.x, size.y, 1.0f });

        submit(transform, tex_index, color);
    }

    void submit(const concepts::vector_least2<float> auto& position,
                const concepts::vector_least2<float> auto& size,
                const concepts::vector_least4<float> auto& color)
    {
        submit(position, size, 0u, color);
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

    void begin_batch(gl::texture2D::id_type texture_id, const glm::mat4& view_proj = glm::mat4(1.0f)) noexcept
    {
        m_renderer.texture_id = texture_id;
        set_view_projection(view_proj);
        reset_current_buffer();
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
        gl::texture2D_array::id_type texture_id;
        gl::texture2D_array default_texture;
        std::vector<fenced_buffer> buffers;
        size_type current_buffer_index{};
        size_type instance_count{};
        glm::mat4 view_proj{};
    };

    renderer_state<instance> m_renderer{};
};

} // namespace flow
