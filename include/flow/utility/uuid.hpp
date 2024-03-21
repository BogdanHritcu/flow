#pragma once

#include <array>
#include <format>

#include "helpers.hpp"

namespace flow {

class uuid
{
public:
    using value_type = std::array<std::uint64_t, 2>;

public:
    constexpr uuid() noexcept
        : m_value{}
    {}

    [[nodiscard]] constexpr const value_type& data() const noexcept
    {
        return m_value;
    }

    [[nodiscard]] static uuid generate();

private:
    constexpr uuid(value_type value) noexcept
        : m_value{ value }
    {}

private:
    value_type m_value;
};

} // namespace flow

template<>
struct std::hash<flow::uuid>
{
    std::size_t operator()(const flow::uuid& uuid) const noexcept
    {
        const std::array<std::uint64_t, 2> data = uuid.data();

        return flow::hash_combine(std::hash<std::uint64_t>{}(data[0]), data[1]);
    }
};
