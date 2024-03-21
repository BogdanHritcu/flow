#include "../../include/flow/core/application.hpp"

#include <glad/gl.h>

#include "../../include/flow/core/logger.hpp"

namespace flow {
input application::input{};

application::application()
    : application(800, 600, "Default title")
{}

application::application(size_type width, size_type height, const std::string& title)
{
    // logger initialization
    auto logger = spdlog::default_logger();
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("%T %^[%=8l]%$: %v");

    FLOW_LOG_INFO("Starting engine");

    // window initialization
    if (!m_window.create(width, height, title))
    {
        FLOW_LOG_CRITICAL("Failed to initialize the window");
        return;
    }

    m_window.set_key_callback(input_callback<key>);
    m_window.set_mouse_button_callback(input_callback<mouse_button>);

    auto framebuffer_resize_callback = [](window& window, size_type width, size_type height)
    {
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    };

    m_window.set_framebuffer_resize_callback(framebuffer_resize_callback);
}

application::~application()
{
    FLOW_LOG_INFO("Terminating engine");
}

void application::start()
{}

void application::update(float dt)
{}

void application::quit() const noexcept
{
    m_window.close();
}

void application::set_window_size(size_type width, size_type height) const noexcept
{
    m_window.set_size(width, height);
}

void application::set_window_title(const std::string& title) const noexcept
{
    m_window.set_title(title);
}

void application::set_window_vsync(bool value) const noexcept
{
    m_window.set_vsync(value);
}

void application::run()
{
    start();

    while (m_window.is_open())
    {
        update(1.0f / 60.0f);

        m_window.swap_buffers();
        m_window.poll_events();
    }
}

} // namespace flow
