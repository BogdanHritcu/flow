#pragma once

#include "engine_interface.hpp"
#include "input_system.hpp"
#include "window.hpp"

namespace flow {

namespace detail {
    int entry(int argc, char** argv);
}

class application
{
private:
    friend int ::flow::detail::entry(int argc, char** argv);

public:
    using size_type = window::size_type;

public:
    application();
    application(const application&) = delete;
    application& operator=(const application&) = delete;
    application(application&&) = delete;
    application& operator=(application&&) = delete;
    virtual ~application() = default;

    virtual void setup()
    {}

    virtual void start()
    {}

    virtual void update(float dt)
    {}

    virtual void end()
    {}

    virtual void cleanup()
    {}

private:
    void run();
    void update_loop();
    bool init();
    void terminate();

private:
    window m_window;
    input_system m_input_system;
    bool m_has_error_state;

protected:
    engine_interface engine; // NOLINT(*-non-private-member-variables-in-classes)
};

} // namespace flow
