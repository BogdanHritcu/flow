#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ranges>
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

class texture_array
{
public:
    using size_type = std::uint32_t;

private:
    struct array_metadata
    {
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t channels;
        std::uint32_t size;
    };

public:
    constexpr texture_array() noexcept = default;

    template<std::ranges::range R>
        requires std::same_as<fs::path, std::ranges::range_value_t<R>>
    texture_array(R&& paths)
    {
        load(paths);
    }

    template<std::ranges::range R>
    bool load(R&& paths)
    {
        std::vector<std::uint8_t> values;
        std::uint32_t width{};
        std::uint32_t height{};
        std::uint32_t size{};

        for (const auto& path : paths)
        {
            image img;
            if (!img.load(path, flow::image_format::rgba))
            {
                return false;
            }

            std::ranges::copy(img.values(), std::back_inserter(values));

            width = img.width();
            height = img.height();
            ++size;
        }

        return create_texture(values, width, height, size);
    }

    [[nodiscard]] constexpr decltype(auto) texture_id() const noexcept
    {
        return m_texture_array.id();
    }

private:
    bool create_texture(std::span<const std::uint8_t> image_data, std::uint32_t width, std::uint32_t height, std::uint32_t size)
    {
        if (!m_texture_array.create())
        {
            return false;
        }

        m_texture_array.storage(1, gl::texture_format::rgba8, width, height, size);
        m_texture_array.sub_image(image_data,
                                  0,
                                  gl::pixel_format::rgba,
                                  gl::to_type_value<std::uint8_t>(),
                                  width,
                                  height,
                                  size);

        m_texture_array.set_filter(gl::texture_filter_direction::minifying, gl::texture_filter_mode::nearest);
        m_texture_array.set_filter(gl::texture_filter_direction::magnifying, gl::texture_filter_mode::nearest);
        m_texture_array.set_wrap(gl::texture_wrap_direction::s, gl::texture_wrap_mode::clamp_to_edge);
        m_texture_array.set_wrap(gl::texture_wrap_direction::t, gl::texture_wrap_mode::clamp_to_edge);

        return true;
    }

private:
    gl::texture2D_array m_texture_array{};
};

} // namespace flow
