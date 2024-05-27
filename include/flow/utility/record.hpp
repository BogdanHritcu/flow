#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>

#include "concepts.hpp"

namespace flow {

template<std::size_t Size>
struct record
{
    std::array<std::byte, Size> storage;

    constexpr record() noexcept
        : storage{}
    {}

    template<concepts::trivially_copyable DataT>
        requires(sizeof(DataT) == Size)
    constexpr record(const DataT& data) noexcept
        : storage{ std::bit_cast<std::array<std::byte, sizeof(DataT)>>(data) }
    {}

    template<concepts::trivially_copyable DataT>
        requires(sizeof(DataT) < Size)
    constexpr record(const DataT& data) noexcept
        : storage{}
    {
        const auto serialized_data = std::bit_cast<std::array<std::byte, sizeof(DataT)>>(data);
        std::copy_n(serialized_data.begin(), serialized_data.size(), storage.begin());
    }

    template<concepts::trivially_copyable DataT>
        requires(sizeof(DataT) <= Size)
    constexpr void from(const DataT& data) noexcept
    {
        if constexpr (sizeof(DataT) == Size)
        {
            storage = std::bit_cast<std::array<std::byte, sizeof(DataT)>>(data);
        }
        else
        {
            const auto serialized_data = std::bit_cast<std::array<std::byte, sizeof(DataT)>>(data);
            std::copy_n(serialized_data.begin(), serialized_data.size(), storage.begin());
        }
    }

    template<concepts::trivially_copyable DataT>
        requires(sizeof(DataT) <= Size)
    [[nodiscard]] constexpr DataT to() const noexcept
    {
        if constexpr (sizeof(DataT) == Size)
        {
            return std::bit_cast<DataT>(storage);
        }
        else
        {
            std::array<std::byte, sizeof(DataT)> serialized_data{};
            std::copy_n(storage.begin(), serialized_data.size(), serialized_data.begin());

            return std::bit_cast<DataT>(serialized_data);
        }
    }
};

} // namespace flow
