#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <utility>
#include <vector>

#include "../../utility/istream_view.hpp"
#include "../../utility/ostream_view.hpp"
#include "../../utility/path_serialization.hpp"
#include "../../utility/serialization.hpp"
#include "../../utility/vector_serialization.hpp"
#include "../opengl/texture.hpp"
#include "image.hpp"

namespace flow {

namespace fs = std::filesystem;

template<typename TextureDataT>
class basic_texture_atlas
{
public:
    using size_type = image::size_type;
    using texture_data_type = TextureDataT;

    struct atlas_metadata
    {
        image_metadata image_metadata;
        fs::path image_path;
        std::vector<texture_data_type> entries;
    };

public:
    constexpr basic_texture_atlas() noexcept = default;

    basic_texture_atlas(const fs::path& path)
    {
        load(path);
    }

    bool load(const fs::path& path)
    {
        std::fstream file(path, std::ios::binary | std::ios::in);
        if (!file)
        {
            return false;
        }

        istream_view in{ file };
        if (!in.deserialize(m_metadata))
        {
            return false;
        }

        image image{};
        if (!image.load(m_metadata.image_path, image_format::rgba))
        {
            return false;
        }
        m_metadata.image_metadata = image.metadata();

        if (!m_texture.create())
        {
            return false;
        }

        m_texture.storage(1, gl::texture_format::rgba8, image.width(), image.height());
        m_texture.sub_image(image.values(),
                            0,
                            gl::pixel_format::rgba,
                            gl::to_type_value<image::value_type>(),
                            image.width(),
                            image.height());

        m_texture.set_filter(gl::texture_filter_direction::minifying, gl::texture_filter_mode::nearest);
        m_texture.set_filter(gl::texture_filter_direction::magnifying, gl::texture_filter_mode::nearest);
        m_texture.set_wrap(gl::texture_wrap_direction::s, gl::texture_wrap_mode::clamp_to_edge);
        m_texture.set_wrap(gl::texture_wrap_direction::t, gl::texture_wrap_mode::clamp_to_edge);

        return true;
    }

    bool save(const fs::path& path) const
    {
        std::fstream file(path, std::ios::binary | std::ios::out);
        if (!file)
        {
            return false;
        }

        ostream_view out{ file };
        if (!out.serialize(m_metadata))
        {
            return false;
        }

        return true;
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return m_metadata.entries.size();
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return m_metadata.entries.empty();
    }

    [[nodiscard]] constexpr size_type width() const noexcept
    {
        return m_metadata.image_metadata.width;
    }

    [[nodiscard]] constexpr size_type height() const noexcept
    {
        return m_metadata.image_metadata.height;
    }

    [[nodiscard]] constexpr size_type channels() const noexcept
    {
        return m_metadata.image_metadata.channels;
    }

    [[nodiscard]] constexpr std::span<const texture_data_type> entries() const noexcept
    {
        return m_metadata.entries;
    }

    [[nodiscard]] constexpr const texture_data_type* entry(size_type index) const noexcept
    {
        return index < m_metadata.entries.size() ? &m_metadata.entries[index] : nullptr;
    }

    void bind() const noexcept
    {
        m_texture.bind();
    }

private:
    gl::texture2D m_texture{};
    atlas_metadata m_metadata;
};

template<typename TextureDataT>
struct deserializer<typename basic_texture_atlas<TextureDataT>::atlas_metadata<TextureDataT>>
{
    void operator()(istream_view in, typename basic_texture_atlas<TextureDataT>::atlas_metadata& m) const
    {
        in.deserialize(m.image_path)
                .deserialize(m.image_metadata)
                .deserialize(m.entries);
    }
};

template<typename TextureDataT>
struct serializer<typename basic_texture_atlas<TextureDataT>::atlas_metadata<TextureDataT>>
{
    void operator()(ostream_view out, const typename basic_texture_atlas<TextureDataT>::atlas_metadata& m) const
    {
        out.serialize(m.image_path)
                .serialize(m.image_metadata)
                .serialize(m.entries);
    }
};

} // namespace flow
