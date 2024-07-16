#pragma once

#include <cstddef>
#include <initializer_list>
#include <span>

#include <glad/gl.h>

#include "../../utility/unique_handle.hpp"
#include "buffer.hpp"
#include "vertex_attribute.hpp"

namespace flow::gl {
class vertex_array
{
public:
    using id_type = GLuint;
    using index_type = GLuint;
    using size_type = std::size_t;
    using offset_type = std::ptrdiff_t;
    using deleter_type = decltype([](id_type id)
    {
        glDeleteVertexArrays(1, &id);
    });
    using handle_type = unique_handle<id_type, deleter_type>;

public:
    constexpr vertex_array() noexcept = default;

    bool create() noexcept
    {
        id_type id{};
        glCreateVertexArrays(1, &id);
        m_handle.reset(id);

        return id != 0;
    }

    void set_vertex_buffer(index_type binding_index,
                           id_type buffer_id,
                           size_type stride,
                           offset_type offset = 0) const noexcept
    {
        glVertexArrayVertexBuffer(static_cast<GLuint>(m_handle.get()),
                                  static_cast<GLuint>(binding_index),
                                  static_cast<GLuint>(buffer_id),
                                  static_cast<GLintptr>(offset * stride),
                                  static_cast<GLsizei>(stride));
    }

    template<typename T>
    void set_vertex_buffer(index_type binding_index, const buffer<T>& buffer, offset_type offset = 0) const noexcept
    {
        set_vertex_buffer(binding_index, buffer.id(), sizeof(T), offset);
    }

    void set_element_buffer(id_type buffer_id) const noexcept
    {
        glVertexArrayElementBuffer(static_cast<GLuint>(m_handle.get()),
                                   static_cast<GLuint>(buffer_id));
    }

    template<typename T>
    void set_element_buffer(const buffer<T>& buffer) const noexcept
    {
        set_element_buffer(buffer.id());
    }

    void set_binding_divisor(index_type binding_index, size_type divisor) const noexcept
    {
        glVertexArrayBindingDivisor(static_cast<GLuint>(m_handle.get()),
                                    static_cast<GLuint>(binding_index),
                                    static_cast<GLuint>(divisor));
    }

    void enable_attribute(index_type attribute_index) const noexcept
    {
        glEnableVertexArrayAttrib(static_cast<GLuint>(m_handle.get()),
                                  static_cast<GLuint>(attribute_index));
    }

    void enable_attributes(std::span<index_type> attribute_indices) const noexcept
    {
        for (auto index : attribute_indices)
        {
            enable_attribute(index);
        }
    }

    void enable_attributes(std::initializer_list<index_type> attribute_indices) const noexcept
    {
        for (auto index : attribute_indices)
        {
            enable_attribute(index);
        }
    }

    void disable_attribute(index_type attribute_index) const noexcept
    {
        glDisableVertexArrayAttrib(static_cast<GLuint>(m_handle.get()),
                                   static_cast<GLuint>(attribute_index));
    }

    void disable_attributes(std::span<index_type> attribute_indices) const noexcept
    {
        for (auto index : attribute_indices)
        {
            disable_attribute(index);
        }
    }

    void disable_attributes(std::initializer_list<index_type> attribute_indices) const noexcept
    {
        for (auto index : attribute_indices)
        {
            disable_attribute(index);
        }
    }

    void set_attribute_config(index_type attribute_index,
                              const attribute_format& format,
                              bool normalize) const noexcept
    {
        glVertexArrayAttribFormat(static_cast<GLuint>(m_handle.get()),
                                  static_cast<GLuint>(attribute_index),
                                  static_cast<GLint>(format.count),
                                  static_cast<GLenum>(format.type),
                                  static_cast<GLboolean>(normalize),
                                  static_cast<GLuint>(format.offset));
    }

    void set_attribute_config(const attribute_config& config) const noexcept
    {
        set_attribute_config(config.index, config.format, config.normalize);
    }

    void set_attribute_binding(index_type attribute_index, index_type binding_index) const noexcept
    {
        glVertexArrayAttribBinding(static_cast<GLuint>(m_handle.get()),
                                   static_cast<GLuint>(attribute_index),
                                   static_cast<GLuint>(binding_index));
    }

    void set_attribute(index_type binding_index,
                       index_type attribute_index,
                       const attribute_format& format,
                       bool normalize,
                       bool enabled = true) const noexcept
    {
        set_attribute_binding(attribute_index, binding_index);
        set_attribute_config(attribute_index, format, normalize);
        if (enabled)
        {
            enable_attribute(attribute_index);
        }
        else
        {
            disable_attribute(attribute_index);
        }
    }

    void set_attribute(index_type binding_index, const attribute_config& config, bool enabled = true) const noexcept
    {
        set_attribute(binding_index, config.index, config.format, config.normalize, enabled);
    }

    void set_attribute_bindings(index_type binding_index, std::span<index_type> attribute_indices) const noexcept
    {
        for (auto index : attribute_indices)
        {
            set_attribute_binding(binding_index, index);
        }
    }

    void set_attribute_bindings(index_type binding_index,
                                std::initializer_list<index_type> attribute_indices) const noexcept
    {
        for (auto index : attribute_indices)
        {
            set_attribute_binding(binding_index, index);
        }
    }

    void bind() const noexcept
    {
        glBindVertexArray(static_cast<GLuint>(m_handle.get()));
    }

    static void unbind() noexcept
    {
        glBindVertexArray(0);
    }

    [[nodiscard]] id_type id() const noexcept
    {
        return m_handle.get();
    }

private:
    handle_type m_handle{};
};
} // namespace flow::gl
