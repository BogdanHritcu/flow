#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "input_system.hpp"

struct GLFWwindow;

namespace flow {

class engine_interface;

namespace detail {

    void destroy(GLFWwindow* ptr) noexcept;
    void terminate() noexcept;

} // namespace detail

class window
{
public:
    using value_type = GLFWwindow;
    using pointer_type = value_type*;
    using size_type = std::uint32_t;
    // clang-format off
    using deleter_type = decltype([](pointer_type ptr) { detail::destroy(ptr); detail::terminate(); });
    // clang-format on
    using handle_type = std::unique_ptr<value_type, deleter_type>;

    struct settings
    {
        size_type width;
        size_type height;
        std::string title;
        bool vsync;
    };

    inline static const window::settings default_settings{
        .width = 640,
        .height = 480,
        .title = "Default title",
        .vsync = false
    };

public:
    constexpr window() noexcept
        : m_handle{}
        , m_window_data{}
        , m_settings{ default_settings }
    {}

    window(engine_interface* engine, const settings& settings) noexcept;

    bool create(engine_interface* engine, const settings& settings) noexcept;
    void close() const noexcept;
    void swap_buffers() const noexcept;
    void poll_events() const noexcept;

    void set_title(std::string_view title) noexcept;
    void set_size(size_type width, size_type height) noexcept;
    void set_vsync(bool value) noexcept;

    [[nodiscard]] bool is_open() const noexcept;

    [[nodiscard]] constexpr std::string_view title() const noexcept
    {
        return m_settings.title;
    }

    [[nodiscard]] constexpr size_type width() const noexcept
    {
        return m_settings.width;
    }

    [[nodiscard]] constexpr size_type height() const noexcept
    {
        return m_settings.height;
    }

    [[nodiscard]] constexpr size_type is_vsync() const noexcept
    {
        return m_settings.vsync;
    }

private:
    void set_callbacks() noexcept;
    void set_key_callback() noexcept;
    void set_mouse_button_callback() noexcept;
    void set_cursor_position_callback() noexcept;
    void set_framebuffer_resize_callback() noexcept;

private:
    struct window_data
    {
        engine_interface* engine;
    };

private:
    handle_type m_handle;
    window_data m_window_data;
    settings m_settings;
};

} // namespace flow
