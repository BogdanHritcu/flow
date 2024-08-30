#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>

#include <stb/stb_image.h>

#include "../../utility/concepts.hpp"

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
    using size_type = std::uint32_t;

    size_type width;
    size_type height;
    size_type channels;
};

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

    bool load(std::span<const std::uint8_t> image_data, image_format force_format = auto_image_format, bool load_flipped = true) noexcept
    {
        pointer_type data = nullptr;
        int width{};
        int height{};
        int channels{};

        stbi_set_flip_vertically_on_load(static_cast<int>(load_flipped));

        auto forced_channels = static_cast<int>(force_format);
        data = stbi_load_from_memory(static_cast<const stbi_uc*>(image_data.data()),
                                     static_cast<int>(image_data.size_bytes()),
                                     &width,
                                     &height,
                                     &channels,
                                     forced_channels);

        if (data)
        {
            m_metadata = {
                .width = static_cast<size_type>(width),
                .height = static_cast<size_type>(height),
                .channels = static_cast<size_type>(forced_channels)
            };

            m_handle.reset(data);
        }

        return data != nullptr;
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

        auto forced_channels = static_cast<int>(force_format);
        data = stbi_load(path.string().c_str(), &width, &height, &channels, forced_channels);

        if (data)
        {
            m_metadata = {
                .width = static_cast<size_type>(width),
                .height = static_cast<size_type>(height),
                .channels = static_cast<size_type>(forced_channels)
            };

            m_handle.reset(data);
        }

        return data != nullptr;
    }

    [[nodiscard]] constexpr std::span<value_type> values() noexcept
    {
        return std::span(m_handle.get(), m_metadata.width * m_metadata.height * m_metadata.channels);
    }

    [[nodiscard]] constexpr std::span<const value_type> values() const noexcept
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

    [[nodiscard]] static std::optional<image_metadata> read_metadata(const fs::path& path) noexcept
    {
        int width{};
        int height{};
        int channels{};

        if (!stbi_info(path.string().c_str(), &width, &height, &channels))
        {
            return std::nullopt;
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

} // namespace flow
