#pragma once

#include <filesystem>
#include <fstream>

#include <glm/vec2.hpp>

#include "../../utility/istream_view.hpp"
#include "../texture/texture_atlas.hpp"

namespace flow {

namespace fs = std::filesystem;

class sprite_animation_atlas
{
public:
    using size_type = image::size_type;

    struct frame_data
    {
        std::uint32_t begin; // ms
        std::uint32_t end;   // ms
        glm::vec2 norm_tex_bottom_left;
        glm::vec2 norm_tex_top_right;
    };

private:
    using texture_atlas_type = basic_texture_atlas<frame_data>;

public:
    constexpr sprite_animation_atlas() = default;

    sprite_animation_atlas(const fs::path& path)
    {
        load(path);
    }

    bool load(const fs::path& path, bool check_frames = true)
    {
        std::fstream file(path, std::ios::binary | std::ios::in);
        return file && load(file);
    }

    bool load(istream_view in, bool check_frames = true)
    {
        if (!m_texture_atlas.load(in))
        {
            return false;
        }

        auto frames = m_texture_atlas.entries();
        if (frames.empty())
        {
            return false;
        }

        if (!check_frames)
        {
            return true;
        }

        std::uint32_t begin = 0;
        for (const auto& f : frames)
        {
            if (f.begin != begin)
            {
                return false;
            }
            begin = f.end;
        }

        return true;
    }

    [[nodiscard]] constexpr const frame_data* frame(size_type index) const noexcept
    {
        return m_texture_atlas.entry(index);
    }

    [[nodiscard]] constexpr std::span<const frame_data> frames() const noexcept
    {
        return m_texture_atlas.entries();
    }

    [[nodiscard]] constexpr size_type frame_count() const noexcept
    {
        return m_texture_atlas.size();
    }

    [[nodiscard]] constexpr const texture_atlas_type& texture_atlas() const noexcept
    {
        return m_texture_atlas;
    }

private:
    texture_atlas_type m_texture_atlas{};
};

} // namespace flow
