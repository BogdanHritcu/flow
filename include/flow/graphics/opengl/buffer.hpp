#pragma once

#include <cstddef>
#include <initializer_list>
#include <span>

#include <glad/gl.h>

#include "../../utility/unique_handle.hpp"

namespace flow {
namespace gl {

    enum class buffer_target : GLenum
    {
        // clang-format off

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

        // clang-format on
    };

    enum class buffer_storage_flags : GLbitfield
    {
        // clang-format off

        none            = 0,
        dynamic_storage = GL_DYNAMIC_STORAGE_BIT,
        map_read        = GL_MAP_READ_BIT,
        map_write       = GL_MAP_WRITE_BIT,
        map_persistent  = GL_MAP_PERSISTENT_BIT,
        map_coherent    = GL_MAP_COHERENT_BIT

        // clang-format on
    };

    enum class buffer_map_flags : GLbitfield
    {
        // clang-format off

        none              = 0,
        read              = GL_MAP_READ_BIT,
        write             = GL_MAP_WRITE_BIT,
        persistent        = GL_MAP_PERSISTENT_BIT,
        coherent          = GL_MAP_COHERENT_BIT,
        invalidate_range  = GL_MAP_INVALIDATE_RANGE_BIT,
        invalidate_buffer = GL_MAP_INVALIDATE_BUFFER_BIT,
        flush_explicit    = GL_MAP_FLUSH_EXPLICIT_BIT,
        unsynchronized    = GL_MAP_UNSYNCHRONIZED_BIT

        // clang-format on
    };

    [[nodiscard]] constexpr buffer_storage_flags operator|(buffer_storage_flags lhs, buffer_storage_flags rhs) noexcept
    {
        return static_cast<buffer_storage_flags>(static_cast<GLbitfield>(lhs) | static_cast<GLbitfield>(rhs));
    }

    [[nodiscard]] constexpr buffer_map_flags operator|(buffer_map_flags lhs, buffer_map_flags rhs) noexcept
    {
        return static_cast<buffer_map_flags>(static_cast<GLbitfield>(lhs) | static_cast<GLbitfield>(rhs));
    }

    template<typename... Ts>
    class buffer;

    template<>
    class buffer<>
    {
    public:
        using id_type = GLuint;
        using index_type = GLuint;
        using size_type = std::size_t;
        using offset_type = std::ptrdiff_t;
        // clang-format off
        using deleter_type = decltype([](id_type id) { glDeleteBuffers(1, &id); });
        // clang-format on
        using handle_type = unique_handle<id_type, deleter_type>;

    public:
        constexpr buffer() noexcept
            : m_handle{}
        {}

        bool create() noexcept
        {
            id_type id{};

            glCreateBuffers(1, &id);
            m_handle.reset(id);

            return id != 0;
        }

        void storage(size_type count, size_type stride, buffer_storage_flags flags) const noexcept
        {
            glNamedBufferStorage(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLsizeiptr>(count * stride),
                                 nullptr,
                                 static_cast<GLbitfield>(flags));
        }

        template<typename T>
        void storage(size_type count, buffer_storage_flags flags) const noexcept
        {
            storage(count, sizeof(T), flags);
        }

        void storage(const void* data, size_type count, size_type stride, buffer_storage_flags flags) const noexcept
        {
            glNamedBufferStorage(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLsizeiptr>(count * stride),
                                 data,
                                 static_cast<GLbitfield>(flags));
        }

        template<typename T>
        void storage(std::span<T> values, buffer_storage_flags flags) const noexcept(noexcept(std::data(values)))
        {
            storage(std::data(values), values.size(), sizeof(T), flags);
        }

        template<typename T>
        void storage(std::initializer_list<T> values, buffer_storage_flags flags) const noexcept(noexcept(std::data(values)))
        {
            storage(std::data(values), values.size(), sizeof(T), flags);
        }

        void sub_data(const void* data, size_type count, size_type stride, offset_type offset = 0) const noexcept
        {
            glNamedBufferSubData(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLintptr>(offset * stride),
                                 static_cast<GLsizeiptr>(count * stride),
                                 data);
        }

        template<typename T>
        void sub_data(std::span<T> values, offset_type offset = 0) const noexcept(noexcept(std::data(values)))
        {
            sub_data(std::data(values), values.size(), sizeof(T), offset);
        }

        template<typename T>
        void sub_data(std::initializer_list<T> values, offset_type offset = 0) const noexcept(noexcept(std::data(values)))
        {
            sub_data(std::data(values), values.size(), sizeof(T), offset);
        }

        void invalidate_data() const noexcept
        {
            glInvalidateBufferData(static_cast<GLuint>(m_handle.get()));
        }

        void invalidate_sub_data(size_type count, size_type stride, offset_type offset = 0) const noexcept
        {
            glInvalidateBufferSubData(static_cast<GLuint>(m_handle.get()),
                                      static_cast<GLintptr>(offset * stride),
                                      static_cast<GLsizeiptr>(count * stride));
        }

        template<typename T>
        void invalidate_sub_data(size_type count, offset_type offset = 0) const noexcept
        {
            invalidate_sub_data(count, sizeof(T), offset);
        }

        [[nodiscard]] void* map(size_type count, size_type stride, offset_type offset, buffer_map_flags flags) const noexcept
        {
            return glMapNamedBufferRange(static_cast<GLuint>(m_handle.get()),
                                         static_cast<GLintptr>(offset * stride),
                                         static_cast<GLsizeiptr>(count * stride),
                                         static_cast<GLbitfield>(flags));
        }

        template<typename T>
        [[nodiscard]] std::span<T> map(size_type count, offset_type offset, buffer_map_flags flags) const noexcept
        {
            T* ptr = static_cast<T*>(map(count, sizeof(T), offset, flags));

            return std::span(ptr, ptr != nullptr ? count : 0);
        }

        bool unmap() const noexcept
        {
            return static_cast<bool>(glUnmapNamedBuffer(static_cast<GLuint>(m_handle.get())));
        }

        void bind(buffer_target target) const noexcept
        {
            glBindBuffer(static_cast<GLenum>(target),
                         static_cast<GLuint>(m_handle.get()));
        }

        void unbind(buffer_target target) const noexcept
        {
            glBindBuffer(static_cast<GLenum>(target), 0);
        }

        void bind_base(buffer_target target, index_type index) const noexcept
        {
            glBindBufferBase(static_cast<GLenum>(target),
                             static_cast<GLuint>(index),
                             static_cast<GLuint>(m_handle.get()));
        }

        void bind_range(buffer_target target, index_type index, size_type count, size_type stride, offset_type offset = 0) const noexcept
        {
            glBindBufferRange(static_cast<GLenum>(target),
                              static_cast<GLuint>(index),
                              static_cast<GLuint>(m_handle.get()),
                              static_cast<GLintptr>(offset * stride),
                              static_cast<GLsizeiptr>(count * stride));
        }

        template<typename T>
        void bind_range(buffer_target target, index_type index, size_type count, offset_type offset = 0) const noexcept
        {
            bind_range(target, index, count, sizeof(T), offset);
        }

        [[nodiscard]] constexpr id_type id() const noexcept
        {
            return m_handle.get();
        }

    private:
        handle_type m_handle;
    };

    template<typename T>
    class buffer<T>
    {
    public:
        using id_type = GLuint;
        using index_type = GLuint;
        using size_type = std::size_t;
        using offset_type = std::ptrdiff_t;
        using value_type = T;
        // clang-format off
        using deleter_type = decltype([](id_type id) { glDeleteBuffers(1, &id); });
        // clang-format on
        using handle_type = unique_handle<id_type, deleter_type>;

    public:
        constexpr buffer() noexcept
            : m_handle{}
        {}

        bool create() noexcept
        {
            id_type id{};

            glCreateBuffers(1, &id);
            m_handle.reset(id);

            return id != 0;
        }

        void storage(size_type count, buffer_storage_flags flags) const noexcept
        {
            glNamedBufferStorage(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLsizeiptr>(count * sizeof(value_type)),
                                 nullptr,
                                 static_cast<GLbitfield>(flags));
        }

        void storage(std::span<value_type> values, buffer_storage_flags flags) const noexcept(noexcept(std::data(values)))
        {
            glNamedBufferStorage(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLsizeiptr>(values.size() * sizeof(value_type)),
                                 std::data(values),
                                 static_cast<GLbitfield>(flags));
        }

        void storage(std::initializer_list<value_type> values, buffer_storage_flags flags) const noexcept(noexcept(std::data(values)))
        {
            glNamedBufferStorage(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLsizeiptr>(values.size() * sizeof(value_type)),
                                 std::data(values),
                                 static_cast<GLbitfield>(flags));
        }

        void sub_data(std::span<value_type> values, offset_type offset = 0) const noexcept(noexcept(std::data(values)))
        {
            glNamedBufferSubData(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLintptr>(offset * sizeof(value_type)),
                                 static_cast<GLsizeiptr>(values.size() * sizeof(value_type)),
                                 std::data(values));
        }

        void sub_data(std::initializer_list<value_type> values, offset_type offset = 0) const noexcept(noexcept(std::data(values)))
        {
            glNamedBufferSubData(static_cast<GLuint>(m_handle.get()),
                                 static_cast<GLintptr>(offset * sizeof(value_type)),
                                 static_cast<GLsizeiptr>(values.size() * sizeof(value_type)),
                                 std::data(values));
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
            value_type* ptr = static_cast<value_type*>(glMapNamedBufferRange(static_cast<GLuint>(m_handle.get()),
                                                                             static_cast<GLintptr>(offset * sizeof(value_type)),
                                                                             static_cast<GLsizeiptr>(count * sizeof(value_type)),
                                                                             static_cast<GLbitfield>(flags)));

            return std::span(ptr, ptr != nullptr ? count : 0);
        }

        bool unmap() const noexcept
        {
            return static_cast<bool>(glUnmapNamedBuffer(static_cast<GLuint>(m_handle.get())));
        }

        void bind(buffer_target target) const noexcept
        {
            glBindBuffer(static_cast<GLenum>(target),
                         static_cast<GLuint>(m_handle.get()));
        }

        void unbind(buffer_target target) const noexcept
        {
            glBindBuffer(static_cast<GLenum>(target), 0);
        }

        void bind_base(buffer_target target, index_type index) const noexcept
        {
            glBindBufferBase(static_cast<GLenum>(target),
                             static_cast<GLuint>(index),
                             static_cast<GLuint>(m_handle.get()));
        }

        void bind_range(buffer_target target, index_type index, size_type count, offset_type offset = 0) const noexcept
        {
            glBindBufferRange(static_cast<GLenum>(target),
                              static_cast<GLuint>(index),
                              static_cast<GLuint>(m_handle.get()),
                              static_cast<GLintptr>(offset * sizeof(value_type)),
                              static_cast<GLsizeiptr>(count * sizeof(value_type)));
        }

        [[nodiscard]] constexpr id_type id() const noexcept
        {
            return m_handle.get();
        }

    private:
        handle_type m_handle;
    };

} // namespace gl
} // namespace flow
