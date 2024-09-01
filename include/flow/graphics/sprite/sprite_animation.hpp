#pragma once

#include <concepts>
#include <filesystem>
#include <fstream>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "../../utility/animation.hpp"
#include "../../utility/time.hpp"
#include "sprite_animation_atlas.hpp"

namespace flow {

namespace fs = std::filesystem;

class sprite_animation : public animation
{
public:
    using size_type = sprite_animation_atlas::size_type;
    using frame_data = sprite_animation_atlas::frame_data;

    struct state
    {
        glm::vec4 color;
        glm::vec2 position;
        glm::vec2 size;
    };

public:
    constexpr sprite_animation() = default;
    constexpr ~sprite_animation() noexcept override = default;

    constexpr sprite_animation(const sprite_animation_atlas* atlas_ptr) noexcept
    {
        set_atlas(atlas_ptr);
    }

    constexpr sprite_animation(const sprite_animation_atlas* atlas_ptr, bool reversed, bool loop) noexcept
        : animation(flow::duration{}, reversed, loop)
    {
        set_atlas(atlas_ptr);
    }

    constexpr void set_atlas(const sprite_animation_atlas* atlas_ptr) noexcept
    {
        m_atlas_ptr = atlas_ptr;

        if (m_atlas_ptr && m_atlas_ptr->frame_count() > 0)
        {
            set_duration(std::chrono::milliseconds{ m_atlas_ptr->frames().back().end });
        }
    }

    [[nodiscard]] constexpr void set_state(const state& state) noexcept
    {
        m_state = state;
    }

    constexpr void restart() noexcept override
    {
        animation::restart();
        m_frame_index = is_reversed() && m_atlas_ptr && m_atlas_ptr->frame_count() > 0 ? frame_count() - 1 : 0;
    }

    [[nodiscard]] constexpr const frame_data* current_frame() const noexcept
    {
        return frame(m_frame_index);
    }

    [[nodiscard]] constexpr const frame_data* frame(size_type index) const noexcept
    {
        return m_atlas_ptr ? m_atlas_ptr->frame(index) : nullptr;
    }

    [[nodiscard]] constexpr size_type frame_count() const noexcept
    {
        return m_atlas_ptr ? m_atlas_ptr->frame_count() : 0;
    }

    [[nodiscard]] constexpr decltype(auto) texture_atlas() const noexcept
    {
        return m_atlas_ptr ? &m_atlas_ptr->texture_atlas() : nullptr;
    }

    [[nodiscard]] constexpr const state& get_state() const noexcept
    {
        return m_state;
    }

protected:
    constexpr void update() override
    {
        flow::duration t = progress();

        const auto* frame_ptr = frame(m_frame_index);
        auto sign = 1 - 2 * static_cast<int>(is_reversed());
        auto count = frame_count();

        while (!has_finished()
               && frame_ptr
               && (t < std::chrono::milliseconds{ frame_ptr->begin }
                   || t >= std::chrono::milliseconds{ frame_ptr->end }))
        {
            m_frame_index = (count + m_frame_index + sign) % count;
            frame_ptr = frame(m_frame_index);
        }
    }

protected:
    state m_state{};

private:
    const sprite_animation_atlas* m_atlas_ptr{};
    size_type m_frame_index = 0;
};

} // namespace flow
