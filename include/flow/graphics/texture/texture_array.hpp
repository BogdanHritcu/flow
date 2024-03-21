#pragma once

#include <cstddef>

#include "../opengl/texture.hpp"
#include "fixed_image_atlas.hpp"
#include "image.hpp"

namespace flow {

namespace fs = std::filesystem;

struct texture_array_config
{
    using size_type = std::size_t;

    size_type width;
    size_type height;
    image_format format;
};

class texture_array
{
public:
    using size_type = texture_array_config::size_type;

public:
    constexpr texture_array() noexcept
        : m_texture{}
    {}

    bool load(const fixed_image_atlas& atlas)
    {
        m_texture.create();
        m_texture.storage(1, // TODO: make mipmap_levels and format not hardcoded
                          gl::texture_format::rgba8,
                          atlas.metadata().width,
                          atlas.metadata().height,
                          atlas.size());

        auto images = atlas.images();

        static constexpr size_type mipmap_level = 0;

        for (size_type layer = 0; layer < images.size(); ++layer)
        {
            m_texture.sub_image(images[layer].span(),
                                mipmap_level,
                                gl::pixel_format::rgba,
                                gl::to_type_value<fixed_image_atlas::value_type>(),
                                images[layer].width(),
                                images[layer].height(),
                                1, // one layer per image
                                0, // xoffset
                                0, // yoffset
                                layer);
        }

        m_texture.set_filter(gl::texture_filter_direction::minifying, gl::texture_filter_mode::linear);
        m_texture.set_filter(gl::texture_filter_direction::magnifying, gl::texture_filter_mode::linear);
        m_texture.set_wrap(gl::texture_wrap_direction::s, gl::texture_wrap_mode::clamp_to_edge);
        m_texture.set_wrap(gl::texture_wrap_direction::t, gl::texture_wrap_mode::clamp_to_edge);

        return true;
    }

    size_type load(const texture_array_config& config, std::span<fs::path> paths)
    {
        fixed_image_atlas atlas;
        size_type loaded_count = atlas.create(config.width, config.height, config.format, paths);

        return loaded_count * static_cast<size_type>(load(atlas));
    }

    void bind() const noexcept
    {
        m_texture.bind();
    }

private:
    gl::texture2D_array m_texture;
};

} // namespace flow
