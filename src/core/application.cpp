#include "../../include/flow/core/application.hpp"

#include "../../include/flow/core/logger.hpp"

namespace flow {

void application::run()
{
    if (init())
    {
        setup();
        start();
        main_loop();
        end();
        cleanup();
    }

    terminate();
}

void application::main_loop()
{
    if (m_fixed_update_frequency == 0)
    {
        m_fixed_update_frequency = 1; // TODO: make this configurable
    }

    using namespace std::chrono_literals;

    time_point last_time = clock::now();
    duration frame_accumulator{};
    const duration fixed_dt = as_nanoseconds_duration(1s) / m_fixed_update_frequency;

    while (m_window.is_open())
    {
        time_point new_time = clock::now();
        duration frame_time = new_time - last_time;
        last_time = new_time;
        frame_accumulator += frame_time;

        while (frame_accumulator >= fixed_dt)
        {
            fixed_update(fixed_dt);
            frame_accumulator -= fixed_dt;
        }

        update(frame_time);

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
