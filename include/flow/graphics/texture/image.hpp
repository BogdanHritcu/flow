#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>

#include <stb/stb_image.h>

#include "../../utility/concepts.hpp"
#include "../bounds.hpp"

namespace flow {

namespace fs = std::filesystem;

enum class image_format
{
    // clang-format off

    grey       = 1,
    grey_alpha = 2,
    rgb        = 3,
    rgba       = 4

    // clang-format on
};

constexpr image_format auto_image_format = static_cast<image_format>(0);

struct image_metadata
{
    using value_type = std::uint8_t;
    using size_type = std::size_t;

    size_type width;
    size_type height;
    size_type channels;
};

[[nodiscard]] constexpr bool operator==(const image_metadata& lhs, const image_metadata& rhs) noexcept
{
    return lhs.width == rhs.width && lhs.height == rhs.height && lhs.channels == rhs.channels;
}

[[nodiscard]] constexpr bool operator!=(const image_metadata& lhs, const image_metadata& rhs) noexcept
{
    return !(lhs == rhs);
}

[[nodiscard]] constexpr bool is_empty_metadata(const image_metadata& metadata) noexcept
{
    return metadata == image_metadata{ 0, 0, 0 };
}

class image
{
public:
    using value_type = std::uint8_t;
    using pointer_type = value_type*;
    using size_type = image_metadata::size_type;
    // clang-format off
    using deleter_type = decltype([](void* ptr) { stbi_image_free(ptr); });
    // clang-format on
    using handle_type = std::unique_ptr<value_type, deleter_type>;

public:
    constexpr image() noexcept
        : m_handle{}
        , m_metadata{}
    {}

    image(const fs::path& path, image_format force_format = auto_image_format, bool load_flipped = true) noexcept
        : m_handle{}
        , m_metadata{}
    {
        load(path, force_format, load_flipped);
    }

    bool load(const fs::path& path, image_format force_format = auto_image_format, bool load_flipped = true) noexcept
    {
        static_assert(std::same_as<pointer_type, stbi_uc*>);

        if (!fs::exists(path) || !fs::is_regular_file(path))
        {
            return false;
        }

        pointer_type data = nullptr;
        int width{};
        int height{};
        int channels{};

        stbi_set_flip_vertically_on_load(static_cast<int>(load_flipped));

        data = stbi_load(path.string().c_str(), &width, &height, &channels, static_cast<int>(force_format));

        if (data)
        {
            m_metadata = {
                .width = static_cast<size_type>(width),
                .height = static_cast<size_type>(height),
                .channels = static_cast<size_type>(channels)
            };

            m_handle.reset(data);
        }

        return data != nullptr;
    }

    [[nodiscard]] pointer_type data() const noexcept
    {
        return m_handle.get();
    }

    [[nodiscard]] std::span<value_type> span() const noexcept
    {
        return std::span(m_handle.get(), m_metadata.width * m_metadata.height * m_metadata.channels);
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

    [[nodiscard]] constexpr image_format format() const noexcept
    {
        return static_cast<image_format>(m_metadata.channels);
    }

    [[nodiscard]] constexpr const image_metadata& metadata() const noexcept
    {
        return m_metadata;
    }

    [[nodiscard]] static std::optional<image_metadata> get_info(const fs::path& path) noexcept
    {
        int width{};
        int height{};
        int channels{};

        if (!stbi_info(path.string().c_str(), &width, &height, &channels))
        {
            return {};
        }

        return image_metadata{
            .width = static_cast<size_type>(width),
            .height = static_cast<size_type>(height),
            .channels = static_cast<size_type>(channels)
        };
    }

private:
    handle_type m_handle;
    image_metadata m_metadata;
};

class image_view
{
public:
    using value_type = image::value_type;
    using pointer_type = value_type*;
    using size_type = image::size_type;

public:
    constexpr image_view() noexcept
        : m_pixel_data{}
        , m_metadata{}
    {}

    constexpr image_view(std::span<value_type> pixel_data, image_metadata& metadata) noexcept
        : m_pixel_data{ pixel_data }
        , m_metadata{ metadata }
    {}

    image_view(const image& image) noexcept
        : m_pixel_data{ image.span() }
        , m_metadata{ image.metadata() }
    {}

    [[nodiscard]] std::span<value_type> span() const noexcept
    {
        return m_pixel_data;
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

    [[nodiscard]] constexpr image_format format() const noexcept
    {
        return static_cast<image_format>(m_metadata.channels);
    }

    [[nodiscard]] constexpr const image_metadata& metadata() const noexcept
    {
        return m_metadata;
    }

private:
    std::span<value_type> m_pixel_data;
    image_metadata m_metadata;
};

} // namespace flow
