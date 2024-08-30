#pragma once

#include <cstddef>
#include <span>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>

#include "../../utility/concepts.hpp"
#include "../../utility/unique_handle.hpp"
#include "enum_types.hpp"

namespace flow::gl {
enum class texture_type : GLenum
{
    texture1D = GL_TEXTURE_1D,
    texture2D = GL_TEXTURE_2D,
    texture3D = GL_TEXTURE_3D,
    texture1D_array = GL_TEXTURE_1D_ARRAY,
    texture2D_array = GL_TEXTURE_2D_ARRAY,
    texture_rectangle = GL_TEXTURE_RECTANGLE
};

enum class texture_format : GLenum
{
    r8 = GL_R8,
    r16 = GL_R16,
    rg8 = GL_RG8,
    rg16 = GL_RG16,
    r3g3b2 = GL_R3_G3_B2,
    rgb4 = GL_RGB4,
    rgb5 = GL_RGB5,
    rgb8 = GL_RGB8,
    rgb10 = GL_RGB10,
    rgb12 = GL_RGB12,
    rgba2 = GL_RGBA2,
    rgba4 = GL_RGBA4,
    rgb5a1 = GL_RGB5_A1,
    rgba8 = GL_RGBA8,
    rgb10a2 = GL_RGB10_A2,
    rgba12 = GL_RGBA12,
    rgba16 = GL_RGBA16,
    srgb8 = GL_SRGB8,
    srgb8a8 = GL_SRGB8_ALPHA8
};

enum class texture_wrap_mode : GLint
{
    repeat = GL_REPEAT,
    mirrored_repeat = GL_MIRRORED_REPEAT,
    clamp_to_edge = GL_CLAMP_TO_EDGE,
    clamp_to_border = GL_CLAMP_TO_BORDER
};

enum class texture_wrap_direction : GLenum
{
    s = GL_TEXTURE_WRAP_S,
    t = GL_TEXTURE_WRAP_T,
    r = GL_TEXTURE_WRAP_R
};

enum class texture_filter_mode : GLint
{
    nearest = GL_NEAREST,
    linear = GL_LINEAR,
    nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
    nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
    linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
    linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR
};

enum class texture_filter_direction : GLenum
{
    minifying = GL_TEXTURE_MIN_FILTER,
    magnifying = GL_TEXTURE_MAG_FILTER
};

enum class pixel_format : GLenum
{
    red = GL_RED,
    rg = GL_RG,
    rgb = GL_RGB,
    bgr = GL_BGR,
    rgba = GL_RGBA,
    bgra = GL_BGRA,
    depth_component = GL_DEPTH_COMPONENT,
    stencil_index = GL_STENCIL_INDEX
};

namespace detail {
    [[nodiscard]] constexpr std::size_t texture_dimensions(texture_type type) noexcept
    {
        switch (type)
        {
            case texture_type::texture1D:         return 1;
            case texture_type::texture2D:         [[fallthrough]];
            case texture_type::texture1D_array:   [[fallthrough]];
            case texture_type::texture_rectangle: return 2;
            case texture_type::texture3D:         [[fallthrough]];
            case texture_type::texture2D_array:   return 3;
            default:                              return 0;
        }
    }

    template<std::size_t N>
    class basic_texture;

    template<>
    class basic_texture<1>
    {
    public:
        using id_type = GLuint;
        using size_type = std::size_t;
        using deleter_type = decltype([](id_type id)
                                      { glDeleteTextures(1, &id); });
        using handle_type = unique_handle<id_type, deleter_type>;

    public:
        constexpr basic_texture() noexcept = default;

        void storage(size_type mipmap_levels, texture_format format, size_type width) const noexcept
        {
            glTextureStorage1D(m_handle.get(),
                               static_cast<GLsizei>(mipmap_levels),
                               static_cast<GLenum>(format),
                               static_cast<GLsizei>(width));
        }

        template<typename T, std::size_t Extent>
        void sub_image(std::span<const T, Extent> values,
                       size_type mipmap_level,
                       pixel_format format,
                       type_value type,
                       size_type width,
                       size_type xoffset = 0) const noexcept(noexcept(std::data(values)))
        {
            glTextureSubImage1D(m_handle.get(),
                                static_cast<GLint>(mipmap_level),
                                static_cast<GLint>(xoffset),
                                static_cast<GLsizei>(width),
                                static_cast<GLenum>(format),
                                static_cast<GLenum>(type),
                                std::data(values));
        }

    protected:
        handle_type m_handle{};
    };

    template<>
    class basic_texture<2>
    {
    public:
        using id_type = GLuint;
        using size_type = std::size_t;
        using deleter_type = decltype([](id_type id)
                                      { glDeleteTextures(1, &id); });
        using handle_type = unique_handle<id_type, deleter_type>;

    public:
        constexpr basic_texture() noexcept = default;

        void storage(size_type mipmap_levels, texture_format format, size_type width, size_type height) const noexcept
        {
            glTextureStorage2D(m_handle.get(),
                               static_cast<GLsizei>(mipmap_levels),
                               static_cast<GLenum>(format),
                               static_cast<GLsizei>(width),
                               static_cast<GLsizei>(height));
        }

        template<typename T, std::size_t Extent>
        void sub_image(std::span<const T, Extent> values,
                       size_type mipmap_level,
                       pixel_format format,
                       type_value type,
                       size_type width,
                       size_type height,
                       size_type xoffset = 0,
                       size_type yoffset = 0) const noexcept(noexcept(std::data(values)))
        {
            glTextureSubImage2D(m_handle.get(),
                                static_cast<GLint>(mipmap_level),
                                static_cast<GLint>(xoffset),
                                static_cast<GLint>(yoffset),
                                static_cast<GLsizei>(width),
                                static_cast<GLsizei>(height),
                                static_cast<GLenum>(format),
                                static_cast<GLenum>(type),
                                std::data(values));
        }

    protected:
        handle_type m_handle{};
    };

    template<>
    class basic_texture<3>
    {
    public:
        using id_type = GLuint;
        using size_type = std::size_t;
        using deleter_type = decltype([](id_type id)
                                      { glDeleteTextures(1, &id); });
        using handle_type = unique_handle<id_type, deleter_type>;

    public:
        constexpr basic_texture() noexcept = default;

        void storage(size_type mipmap_levels,
                     texture_format format,
                     size_type width,
                     size_type height,
                     size_type depth) const noexcept
        {
            glTextureStorage3D(m_handle.get(),
                               static_cast<GLsizei>(mipmap_levels),
                               static_cast<GLenum>(format),
                               static_cast<GLsizei>(width),
                               static_cast<GLsizei>(height),
                               static_cast<GLsizei>(depth));
        }

        template<typename T, std::size_t Extent>
        void sub_image(std::span<const T, Extent> values,
                       size_type mipmap_level,
                       pixel_format format,
                       type_value type,
                       size_type width,
                       size_type height,
                       size_type depth,
                       size_type xoffset = 0,
                       size_type yoffset = 0,
                       size_type zoffset = 0) const noexcept(noexcept(std::data(values)))
        {
            glTextureSubImage3D(m_handle.get(),
                                static_cast<GLint>(mipmap_level),
                                static_cast<GLint>(xoffset),
                                static_cast<GLint>(yoffset),
                                static_cast<GLint>(zoffset),
                                static_cast<GLsizei>(width),
                                static_cast<GLsizei>(height),
                                static_cast<GLsizei>(depth),
                                static_cast<GLenum>(format),
                                static_cast<GLenum>(type),
                                std::data(values));
        }

    protected:
        handle_type m_handle{};
    };
} // namespace detail

template<texture_type Type>
class basic_texture final : public detail::basic_texture<detail::texture_dimensions(Type)>
{
public:
    using id_type = typename detail::basic_texture<detail::texture_dimensions(Type)>::id_type;
    using size_type = typename detail::basic_texture<detail::texture_dimensions(Type)>::size_type;

public:
    bool create() noexcept
    {
        GLuint id;
        glCreateTextures(static_cast<GLenum>(Type), 1, &id);

        m_handle.reset(id);

        return id != 0;
    }

    void set_filter(texture_filter_direction direction, texture_filter_mode mode) const noexcept
    {
        glTextureParameteri(m_handle.get(), static_cast<GLenum>(direction), static_cast<GLint>(mode));
    }

    void set_wrap(texture_wrap_direction direction, texture_wrap_mode mode) noexcept
    {
        glTextureParameteri(m_handle.get(), static_cast<GLenum>(direction), static_cast<GLint>(mode));
    }

    void set_wrap_border_color(const glm::vec4& color) noexcept
    {
        glTextureParameterfv(m_handle.get(), GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
    }

    void bind() const noexcept
    {
        glBindTexture(static_cast<GLenum>(Type), m_handle.get());
    }

    [[nodiscard]] constexpr id_type id() const noexcept
    {
        return m_handle.get();
    }

    static void bind(id_type id) noexcept
    {
        glBindTexture(static_cast<GLenum>(Type), id);
    }

    static void unbind() noexcept
    {
        glBindTexture(static_cast<GLenum>(Type), 0);
    }

private:
    using detail::basic_texture<detail::texture_dimensions(Type)>::m_handle;
};

using texture1D = basic_texture<texture_type::texture1D>;
using texture2D = basic_texture<texture_type::texture2D>;
using texture3D = basic_texture<texture_type::texture3D>;
using texture = texture2D;

using texture1D_array = basic_texture<texture_type::texture1D_array>;
using texture2D_array = basic_texture<texture_type::texture2D_array>;
using texture_array = texture2D_array;
} // namespace flow::gl
