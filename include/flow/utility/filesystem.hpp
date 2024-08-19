#pragma once

#include <filesystem>
#include <fstream>

namespace flow {

namespace fs = std::filesystem;

[[nodiscard]] bool file_has_read_write_permissions(const fs::path& path) noexcept
{
    std::error_code error{};
    auto status = fs::status(path, error);
    auto perms = status.permissions() & (fs::perms::owner_write | fs::perms::owner_read);

    return fs::exists(status)
            && fs::is_regular_file(status)
            && perms != fs::perms::none;
}

bool create_path(const fs::path& path, bool delete_existing_file = false)
{
    std::error_code error{};
    auto parent_path = path.parent_path();
    if (fs::exists(parent_path) && !fs::is_directory(parent_path))
    {
        return false;
    }

    if (fs::create_directories(parent_path, error); error)
    {
        return false;
    }

    std::fstream file{ path, delete_existing_file ? std::ios::out | std::ios::trunc : std::ios::out };

    return static_cast<bool>(file);
}

} // namespace flow
