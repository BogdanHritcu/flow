#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <utility>
#include <vector>

#include <glm/vec2.hpp>

#include "../../utility/istream_view.hpp"
#include "../../utility/serialization.hpp"
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
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t channels;
        std::uint32_t entry_count;
    };

public:
    constexpr basic_texture_atlas() noexcept = default;

    basic_texture_atlas(const fs::path& path)
    {
        load(path);
    }

    basic_texture_atlas(istream_view in)
    {
        load(in);
    }

    bool load(const fs::path& path)
    {
        std::fstream file(path, std::ios::binary | std::ios::in);
        return file && load(file);
    }

    bool load(istream_view in)
    {
        if (!in.deserialize(m_metadata))
        {
            return false;
        }

        m_entries.resize(m_metadata.entry_count);
        if (!in.deserialize(std::span{ m_entries }))
        {
            return false;
        }

        std::vector<std::uint8_t> image_data;
        image_data.resize(m_metadata.width * m_metadata.height * m_metadata.channels);
        if (!in.deserialize(std::span{ image_data }))
        {
            return false;
        }

        return create_texture(image_data, m_metadata.width, m_metadata.height);
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return m_entries.size();
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return m_entries.empty();
    }

    [[nodiscard]] constexpr size_type width() const noexcept
    {
        return m_metadata.width;
    }

    [[nodiscard]] constexpr size_type height() const noexcept
    {
        return m_metadata.height;
    }

    [[nodiscard]] constexpr size_type channels() const noexcept
    {
        return m_metadata.channels;
    }

    [[nodiscard]] constexpr std::span<const texture_data_type> entries() const noexcept
    {
        return m_entries;
    }

    [[nodiscard]] constexpr const texture_data_type* entry(size_type index) const noexcept
    {
        return index < m_entries.size() ? &m_entries[index] : nullptr;
    }

    [[nodiscard]] constexpr decltype(auto) texture_id() const noexcept
    {
        return m_texture.id();
    }

private:
    bool create_texture(std::span<const std::uint8_t> image_data, std::uint32_t width, std::uint32_t height)
    {
        if (!m_texture.create())
        {
            return false;
        }

        m_texture.storage(1, gl::texture_format::rgba8, width, height);
        m_texture.sub_image(image_data,
                            0,
                            gl::pixel_format::rgba,
                            gl::to_type_value<std::uint8_t>(),
                            width,
                            height);

        m_texture.set_filter(gl::texture_filter_direction::minifying, gl::texture_filter_mode::nearest);
        m_texture.set_filter(gl::texture_filter_direction::magnifying, gl::texture_filter_mode::nearest);
        m_texture.set_wrap(gl::texture_wrap_direction::s, gl::texture_wrap_mode::clamp_to_edge);
        m_texture.set_wrap(gl::texture_wrap_direction::t, gl::texture_wrap_mode::clamp_to_edge);

        return true;
    }

private:
    gl::texture2D m_texture{};
    atlas_metadata m_metadata{};
    std::vector<texture_data_type> m_entries{};
};

struct tileset_entry
{
    glm::vec2 norm_tex_bottom_left;
    glm::vec2 norm_tex_top_right;
};

using tileset_atlas = basic_texture_atlas<tileset_entry>;

} // namespace flow
