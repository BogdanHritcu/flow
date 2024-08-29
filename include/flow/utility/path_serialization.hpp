#pragma once

#include <filesystem>

#include "istream_view.hpp"
#include "ostream_view.hpp"
#include "serialization.hpp"
#include "string_serialization.hpp"

namespace flow {

struct deserializer<std::filesystem::path>
{
    void operator()(istream_view in, std::filesystem::path& path) const
    {
        std::u8string str{};
        if (!in.deserialize(str))
        {
            return;
        }

        path = std::filesystem::path{ str };
    }
};

struct serializer<std::filesystem::path>
{
    void operator()(ostream_view out, const std::filesystem::path& path) const
    {
        out.serialize(path.u8string());
    }
};

} // namespace flow
