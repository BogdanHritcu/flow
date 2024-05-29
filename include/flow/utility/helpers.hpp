#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "concepts.hpp"

namespace flow {

template<typename T>
[[nodiscard]] constexpr std::size_t hash_combine(std::size_t seed, const T& value) noexcept(concepts::nothrow_hashable<T>)
{
    return seed ^ (std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2)); // NOLINT(*-avoid-magic-numbers)
}

struct string_hash
{
    using hash_type = std::hash<std::string_view>;
    using is_transparent = void;

    std::size_t operator()(const char* str) const
    {
        return hash_type{}(str);
    }

    std::size_t operator()(std::string_view str) const
    {
        return hash_type{}(str);
    }

    std::size_t operator()(const std::string& str) const
    {
        return hash_type{}(str);
    }
};

template<typename T>
using unordered_string_map = std::unordered_map<std::string,
                                                T,
                                                string_hash,
                                                std::equal_to<>>;

} // namespace flow
