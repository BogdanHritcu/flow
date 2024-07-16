#pragma once

#include <concepts>
#include <cstddef>
#include <ranges>
#include <span>

#include <glad/gl.h>

#include "../../utility/concepts.hpp"
#include "../../utility/unique_handle.hpp"

namespace flow::gl {
enum class buffer_target : GLenum
{
    array              = GL_ARRAY_BUFFER,
    element_array      = GL_ELEMENT_ARRAY_BUFFER,
    shader_storage     = GL_SHADER_STORAGE_BUFFER,
    texture            = GL_TEXTURE_BUFFER,
    transform_feedback = GL_TRANSFORM_FEEDBACK_BUFFER,
    uniform            = GL_UNIFORM_BUFFER,
    draw_indirect      = GL_DRAW_INDIRECT_BUFFER,
    dispatch_indirect  = GL_DISPATCH_INDIRECT_BUFFER,
    atomic_counter     = GL_ATOMIC_COUNTER_BUFFER,
    pixel_pack         = GL_PIXEL_PACK_BUFFER,
    pixel_unpack       = GL_PIXEL_UNPACK_BUFFER,
    copy_read          = GL_COPY_READ_BUFFER,
    copy_write         = GL_COPY_WRITE_BUFFER,
    query              = GL_QUERY_BUFFER
};

enum class buffer_storage_flags : GLbitfield
{
    none            = 0,
    dynamic_storage = GL_DYNAMIC_STORAGE_BIT,
    map_read        = GL_MAP_READ_BIT,
    map_write       = GL_MAP_WRITE_BIT,
    map_persistent  = GL_MAP_PERSISTENT_BIT,
    map_coherent    = GL_MAP_COHERENT_BIT
};

enum class buffer_map_flags : GLbitfield
{
    none              = 0,
    read              = GL_MAP_READ_BIT,
    write             = GL_MAP_WRITE_BIT,
    persistent        = GL_MAP_PERSISTENT_BIT,
    coherent          = GL_MAP_COHERENT_BIT,
    invalidate_range  = GL_MAP_INVALIDATE_RANGE_BIT,
    invalidate_buffer = GL_MAP_INVALIDATE_BUFFER_BIT,
    flush_explicit    = GL_MAP_FLUSH_EXPLICIT_BIT,
    unsynchronized    = GL_MAP_UNSYNCHRONIZED_BIT
};

[[nodiscard]] constexpr buffer_storage_flags operator|(buffer_storage_flags lhs, buffer_storage_flags rhs) noexcept
{
    return static_cast<buffer_storage_flags>(static_cast<GLbitfield>(lhs) | static_cast<GLbitfield>(rhs));
}

[[nodiscard]] constexpr buffer_map_flags operator|(buffer_map_flags lhs, buffer_map_flags rhs) noexcept
{
    return static_cast<buffer_map_flags>(static_cast<GLbitfield>(lhs) | static_cast<GLbitfield>(rhs));
}

template<concepts::trivially_copyable T>
class buffer
{
public:
    using id_type = GLuint;
    using index_type = GLuint;
    using size_type = std::size_t;
    using offset_type = std::ptrdiff_t;
    using value_type = T;
    using deleter_type = decltype([](id_type id)
    {
        glDeleteBuffers(1, &id);
    });

    using handle_type = unique_handle<id_type, deleter_type>;

public:
    constexpr buffer() noexcept = default;

    bool create() noexcept
    {
        id_type id{};

        glCreateBuffers(1, &id);
        m_handle.reset(id);

        return id != 0;
    }

    void storage(size_type count, buffer_storage_flags flags) const noexcept
    {
        glNamedBufferStorage(static_cast<GLuint>(m_handle.get()), static_cast<GLsizeiptr>(count * sizeof(value_type)),
                             nullptr, static_cast<GLbitfield>(flags));
    }

    template<std::ranges::contiguous_range R>
        requires std::same_as<std::ranges::range_value_t<R>, value_type>
    void storage(R&& r, buffer_storage_flags flags) const noexcept(noexcept(std::ranges::data(r)))
    {
        glNamedBufferStorage(static_cast<GLuint>(m_handle.get()),
                             static_cast<GLsizeiptr>(std::ranges::ssize(r) * sizeof(value_type)), std::ranges::data(r),
                             static_cast<GLbitfield>(flags));
    }

    void storage(const value_type& data, buffer_storage_flags flags) const noexcept
    {
        glNamedBufferStorage(static_cast<GLuint>(m_handle.get()), static_cast<GLsizeiptr>(sizeof(value_type)), &data,
                             static_cast<GLbitfield>(flags));
    }

    template<std::ranges::contiguous_range R>
        requires std::same_as<std::ranges::range_value_t<R>, value_type>
    void sub_data(R&& r, offset_type offset = 0) const noexcept(noexcept(std::ranges::data(r)))
    {
        glNamedBufferSubData(static_cast<GLuint>(m_handle.get()), static_cast<GLintptr>(offset * sizeof(value_type)),
                             static_cast<GLsizeiptr>(std::ranges::ssize(r) * sizeof(value_type)), std::ranges::data(r));
    }

    void sub_data(const value_type& data, offset_type offset = 0) const noexcept
    {
        glNamedBufferSubData(static_cast<GLuint>(m_handle.get()), static_cast<GLintptr>(offset * sizeof(value_type)),
                             static_cast<GLsizeiptr>(sizeof(value_type)), &data);
    }

    void invalidate_data() const noexcept
    {
        glInvalidateBufferData(static_cast<GLuint>(m_handle.get()));
    }

    void invalidate_sub_data(size_type count, offset_type offset = 0) const noexcept
    {
        glInvalidateBufferSubData(static_cast<GLuint>(m_handle.get()),
                                  static_cast<GLintptr>(offset * sizeof(value_type)),
                                  static_cast<GLsizeiptr>(count * sizeof(value_type)));
    }

    [[nodiscard]] std::span<value_type> map(size_type count, offset_type offset, buffer_map_flags flags) const noexcept
    {
        auto* ptr = static_cast<value_type*>(glMapNamedBufferRange(
                static_cast<GLuint>(m_handle.get()), static_cast<GLintptr>(offset * sizeof(value_type)),
                static_cast<GLsizeiptr>(count * sizeof(value_type)), static_cast<GLbitfield>(flags)));

        return std::span(ptr, ptr != nullptr ? count : 0);
    }

    bool unmap() const noexcept
    {
        return static_cast<bool>(glUnmapNamedBuffer(static_cast<GLuint>(m_handle.get())));
    }

    void bind(buffer_target target) const noexcept
    {
        glBindBuffer(static_cast<GLenum>(target), static_cast<GLuint>(m_handle.get()));
    }

    static void unbind(buffer_target target) noexcept
    {
        glBindBuffer(static_cast<GLenum>(target), 0);
    }

    void bind_base(buffer_target target, index_type index) const noexcept
    {
        glBindBufferBase(static_cast<GLenum>(target), static_cast<GLuint>(index), static_cast<GLuint>(m_handle.get()));
    }

    void bind_range(buffer_target target, index_type index, size_type count, offset_type offset = 0) const noexcept
    {
        glBindBufferRange(static_cast<GLenum>(target), static_cast<GLuint>(index), static_cast<GLuint>(m_handle.get()),
                          static_cast<GLintptr>(offset * sizeof(value_type)),
                          static_cast<GLsizeiptr>(count * sizeof(value_type)));
    }

    [[nodiscard]] constexpr id_type id() const noexcept
    {
        return m_handle.get();
    }

private:
    handle_type m_handle{};
};
} // namespace flow::gl
