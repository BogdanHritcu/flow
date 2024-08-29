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

class texture_atlas
{
public:
    using size_type = image::size_type;
    using value_type = image::value_type;

    struct atlas_metadata
    {
        struct entry
        {
            size_type x;
            size_type y;
            size_type width;
            size_type height;
        };

        image_metadata image_metadata;
        fs::path image_path;
        std::vector<entry> entries;
    };

public:
    constexpr texture_atlas() noexcept = default;

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

        ostream_view ou{ file };
        if (!ou.serialize(m_metadata))
        {
            return false;
        }

        return true;
    }

    [[nodiscard]] constexpr std::optional<atlas_metadata::entry> get_entry(size_type index) const noexcept
    {
        return index < m_metadata.entries.size() ? std::optional{ m_metadata.entries[index] } : std::nullopt;
    }

    void bind() const noexcept
    {
        m_texture.bind();
    }

private:
    gl::texture2D m_texture{};
    atlas_metadata m_metadata;
};

struct deserializer<texture_atlas::atlas_metadata>
{
    void operator()(istream_view in, texture_atlas::atlas_metadata& m) const
    {
        in.deserialize(m.image_metadata)
                .deserialize(m.image_path)
                .deserialize(m.entries);
    }
};

struct serializer<texture_atlas::atlas_metadata>
{
    void operator()(ostream_view out, const texture_atlas::atlas_metadata& m) const
    {
        out.serialize(m.image_metadata)
                .serialize(m.image_path)
                .serialize(m.entries);
    }
};

} // namespace flow
