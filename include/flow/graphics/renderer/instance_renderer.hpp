#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "../../core/assertion.hpp"
#include "../opengl/buffer.hpp"
#include "../opengl/commands.hpp"
#include "../opengl/fence.hpp"
#include "../opengl/vertex_array.hpp"
#include "renderer_config.hpp"

namespace flow {

template<typename InstanceT>
class instance_renderer
{
public:
    using instance_type = InstanceT;

public:
    constexpr instance_renderer() noexcept
        : m_renderer{}
        , m_batch{}
    {
    }

    template<typename VertexT, typename IndexT>
    bool create(const renderer_config<VertexT, IndexT>& config) noexcept
    {
        if (!(m_renderer.ssbo.create()
              && m_renderer.vao.create()
              && m_renderer.vbo.create()
              && m_renderer.ebo.create()))
        {
            return false;
        }

        FLOW_ASSERT(config.capacity > 0, "invalid buffer capacity");
        FLOW_ASSERT(config.buffer_count > 0, "invalid buffer count");

        m_renderer.instance_capacity = config.capacity;
        m_renderer.buffers.resize(config.buffer_count);

        using sf = flow::gl::buffer_storage_flags;
        using mf = flow::gl::buffer_map_flags;

        std::size_t ssbo_size = m_renderer.instance_capacity * m_renderer.buffers.size();
        auto ssbo_storage_flags = sf::dynamic_storage | sf::map_write | sf::map_persistent | sf::map_coherent;
        m_renderer.ssbo.storage(ssbo_size, ssbo_storage_flags);

        auto ssbo_map_flags = mf::write | mf::persistent | mf::coherent;
        auto ssbo_span = m_renderer.ssbo.map(ssbo_size, 0, ssbo_map_flags);

        for (std::size_t i = 0; i < m_renderer.buffers.size(); ++i)
        {
            m_renderer.buffers[i].instances = ssbo_span.subspan(i * m_renderer.instance_capacity,
                                                                m_renderer.instance_capacity);
        }

        static constexpr auto binding_index = 0;

        m_renderer.vbo.storage(config.vertices, sf::none);
        m_renderer.vao.set_vertex_buffer(binding_index, m_renderer.vbo, sizeof(VertexT));

        for (auto& attribute : config.attributes)
        {
            m_renderer.vao.set_attribute(binding_index, attribute, true);
        }

        m_renderer.ebo.storage(config.indices, sf::none);
        m_renderer.vao.set_element_buffer(m_renderer.ebo);
        m_renderer.draw_config.element_type_value = gl::to_type_value<IndexT>();

        m_renderer.draw_config.primitive_type = gl::to_primitive_type(config.render_mode);
        m_renderer.draw_config.element_count = config.indices.size();
        m_renderer.draw_config.element_offset = 0;

        return true;
    }

    void begin_batch() noexcept
    {
        m_batch.current_instance_count = 0;
        m_renderer.buffers[m_batch.active_buffer_index].fence.wait(); // wait for the previous draw command on this buffer region
    }

    void submit(const instance_type& instance)
    {
        if (m_batch.current_instance_count >= m_renderer.instance_capacity)
        {
            end_batch();
            begin_batch();
        }

        auto& active_buffer = m_renderer.buffers[m_batch.active_buffer_index].instances;
        active_buffer[m_batch.current_instance_count] = instance;

        ++m_batch.current_instance_count;
    }

    void submit(instance_type&& instance)
    {
        if (m_batch.current_instance_count >= m_renderer.instance_capacity)
        {
            end_batch();
            begin_batch();
        }

        auto& active_buffer = m_renderer.buffers[m_batch.active_buffer_index].instances;
        active_buffer[m_batch.current_instance_count] = instance;

        ++m_batch.current_instance_count;
    }

    void end_batch() noexcept
    {
        if (m_batch.current_instance_count > 0)
        {
            m_renderer.ssbo.bind_range(gl::buffer_target::shader_storage,
                                       0,
                                       m_batch.current_instance_count,
                                       m_batch.active_buffer_index * m_renderer.instance_capacity);
            m_renderer.vao.bind();
            gl::draw_elements_instanced(m_batch.current_instance_count,
                                        m_renderer.draw_config.primitive_type,
                                        m_renderer.draw_config.element_type_value,
                                        m_renderer.draw_config.element_count,
                                        m_renderer.draw_config.element_offset);
            m_renderer.buffers[m_batch.active_buffer_index].fence.lock();
            m_batch.active_buffer_index = ++m_batch.active_buffer_index % m_renderer.buffers.size();
        }
    }

private:
    struct renderer_state
    {
        struct buffer_region
        {
            std::span<instance_type> instances;
            gl::fence fence;
        };

        struct draw_config
        {
            gl::type_value element_type_value;
            gl::primitive_type primitive_type;
            std::size_t element_count;
            std::size_t element_offset;
        };

        std::size_t instance_capacity{};
        std::vector<buffer_region> buffers;
        gl::buffer<instance_type> ssbo;
        gl::vertex_array vao;
        gl::buffer<> vbo;
        gl::buffer<> ebo;
        draw_config draw_config;
    };

    struct batch_state
    {
        std::size_t current_instance_count;
        std::size_t active_buffer_index;
    };

    renderer_state m_renderer;
    batch_state m_batch;
};

} // namespace flow
