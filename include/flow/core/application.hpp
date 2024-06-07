#pragma once

#include "engine_interface.hpp"
#include "input_system.hpp"
#include "window.hpp"

#include "../utility/time.hpp"

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
    constexpr application() noexcept = default;
    application(const application&) = delete;
    application& operator=(const application&) = delete;
    application(application&&) = delete;
    application& operator=(application&&) = delete;
    virtual ~application() = default;

    virtual void setup()
    {}

    virtual void start()
    {}

    virtual void update(duration dt)
    {}

    virtual void fixed_update(duration dt)
    {}

    virtual void end()
    {}

    virtual void cleanup()
    {}

private:
    void run();
    void main_loop();
    bool init();
    void terminate();

private:
    window m_window{};
    input_system m_input_system{};
    bool m_has_error_state{};

    std::uint32_t m_fixed_update_frequency{};

protected:
    engine_interface engine{}; // NOLINT(*-non-private-member-variables-in-classes)
};

} // namespace flow
