#pragma once

#include <algorithm>
#include <span>

#include "../bounds.hpp"
#include "image.hpp"

namespace flow {

namespace fs = std::filesystem;

class fixed_image_atlas
{
public:
    using size_type = image::size_type;
    using value_type = image::value_type;

public:
    constexpr fixed_image_atlas() noexcept
        : m_images{}
        , m_image_metadata{}
    {
    }

    bool create(size_type width, size_type height, image_format format) noexcept
    {
        m_image_metadata = {
            .width = width,
            .height = height,
            .channels = static_cast<size_type>(format)
        };

        m_images.clear();

        return true;
    }

    size_type create(size_type width, size_type height, image_format format, std::span<fs::path> paths)
    {
        create(width, height, format);

        return load(paths);
    }

    size_type load(std::span<fs::path> paths)
    {
        if (is_empty_metadata(m_image_metadata))
        {
            return 0;
        }

        m_images.reserve(paths.size());

        size_type load_counter = 0;

        for (auto& path : paths)
        {
            load_counter += static_cast<size_type>(add_image(path));
        }

        return load_counter;
    }

    bool add_image(const fs::path& path)
    {
        image img;
        if (!img.load(path, static_cast<image_format>(m_image_metadata.channels)) || img.metadata() != m_image_metadata)
        {
            return false;
        }

        m_images.emplace_back(std::move(img));

        return true;
    }

    [[nodiscard]] constexpr std::span<image> images() noexcept
    {
        return m_images;
    }

    [[nodiscard]] constexpr std::span<const image> images() const noexcept
    {
        return m_images;
    }

    [[nodiscard]] constexpr image_metadata metadata() const noexcept
    {
        return m_image_metadata;
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return m_images.size();
    }

private:
    std::vector<image> m_images;
    image_metadata m_image_metadata;
};

} // namespace flow
