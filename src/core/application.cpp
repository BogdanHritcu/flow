#include "../../include/flow/core/application.hpp"

#include "../../include/flow/core/application.hpp"
#include "../../include/flow/core/logger.hpp"
#include "include/flow/core/engine_interface.hpp"

namespace flow {

application::application()
    : m_window{}
    , m_input_system{}
    , m_has_error_state{ false }
    , engine{}
{}

void application::run()
{
    if (init())
    {
        setup();
        start();
        update_loop();
        end();
        cleanup();
    }

    terminate();
}

void application::update_loop()
{
    while (m_window.is_open())
    {
        update(1.0f / 60.0f);

        m_window.swap_buffers();
        m_window.poll_events();
    }
}

bool application::init()
{
    // logger initialization
    auto logger = spdlog::default_logger();
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("%T %^[%=8l]%$: %v");

    FLOW_LOG_INFO("Starting engine");

    // window initialization
    if (!m_window.create(&engine, window::default_settings))
    {
        FLOW_LOG_CRITICAL("Failed to initialize the window");

        return false;
    }

    engine = engine_interface(m_window, m_input_system);

    return true;
}

void application::terminate()
{
    FLOW_LOG_INFO("Terminating engine");
}

} // namespace flow
