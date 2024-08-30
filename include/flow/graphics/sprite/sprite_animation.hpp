#pragma once

#include <concepts>
#include <filesystem>
#include <fstream>
#include <vector>

#include <glm/vec2.hpp>

#include "../../utility/animation.hpp"
#include "../../utility/istream_view.hpp"
#include "../../utility/ostream_view.hpp"
#include "../../utility/serialization.hpp"
#include "../../utility/time.hpp"
#include "../texture/texture_atlas.hpp"

namespace flow {

namespace fs = std::filesystem;

class sprite_animation : public animation
{
public:
    using size_type = image::size_type;

    struct frame_data
    {
        std::uint32_t duration; // ms
        glm::vec2 norm_tex_bottom_left;
        glm::vec2 norm_tex_top_right;
    };

private:
    using texture_atlas_type = basic_texture_atlas<frame_data>;

public:
    constexpr sprite_animation() = default;
    constexpr ~sprite_animation() noexcept override = default;

    sprite_animation(const fs::path& path)
    {
        load(path);
    }

    bool load(const fs::path& path)
    {
        std::fstream file(path, std::ios::binary | std::ios::in);
        return file && load(file);
    }

    bool load(istream_view in)
    {
        if (!m_texture_atlas.load(in))
        {
            return false;
        }

        flow::duration duration{};
        for (const auto& e : m_texture_atlas.entries())
        {
            duration += std::chrono::milliseconds(e.duration);
        }
        set_duration(duration);

        return true;
    }

    constexpr void reset() noexcept override
    {
        animation::reset();
        m_current_frame_index = 0;
        m_prev_frame_end = {};
    }

    [[nodiscard]] constexpr const frame_data* current_frame() const noexcept
    {
        return frame(m_current_frame_index);
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

protected:
    constexpr void update() override
    {
        flow::duration t = progress();

        const auto* frame_ptr = frame(m_current_frame_index);
        while (frame_ptr && t >= m_prev_frame_end + std::chrono::milliseconds(frame_ptr->duration))
        {
            m_prev_frame_end += std::chrono::milliseconds(frame_ptr->duration);
            frame_ptr = frame(++m_current_frame_index);
        }
    }

private:
    texture_atlas_type m_texture_atlas{};
    size_type m_current_frame_index = 0;
    flow::duration m_prev_frame_end{};
};

} // namespace flow
