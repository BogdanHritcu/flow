#pragma once

#include "window.hpp"

namespace flow {

class window_interface
{
public:
    using size_type = window::size_type;

public:
    constexpr window_interface()
        : m_window{}
    {}

    constexpr window_interface(window& window)
        : m_window{ &window }
    {}

    void set_title(std::string_view title) noexcept
    {
        m_window->set_title(title);
    }

    void set_size(size_type width, size_type height) noexcept
    {
        m_window->set_size(width, height);
    }

    void set_vsync(bool value) noexcept
    {
        m_window->set_vsync(value);
    }

    [[nodiscard]] bool is_open() const noexcept
    {
        return m_window->is_open();
    }

    [[nodiscard]] constexpr std::string_view title() const noexcept
    {
        return m_window->title();
    }

    [[nodiscard]] constexpr size_type width() const noexcept
    {
        return m_window->width();
    }

    [[nodiscard]] constexpr size_type height() const noexcept
    {
        return m_window->height();
    }

    [[nodiscard]] constexpr size_type is_vsync() const noexcept
    {
        return m_window->is_vsync();
    }

private:
    window* m_window;
};

} // namespace flow
