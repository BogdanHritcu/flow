#pragma once

#include "input_system_interface.hpp"
#include "window_interface.hpp"

namespace flow {

class engine_interface
{
public:
    static constexpr auto key = input_system_interface::key;
    static constexpr auto mbtn = input_system_interface::mbtn;
    static constexpr auto action = input_system_interface::action;
    static constexpr auto mod = input_system_interface::mod;

public:
    constexpr engine_interface()
        : window{}
        , input{}
        , m_window{}
        , m_input{}
    {}

    constexpr engine_interface(window& window, input_system& input)
        : window{ window }
        , input{ input }
        , m_window{ &window }
        , m_input{ &input }
    {}

    void quit() const noexcept
    {
        m_window->close();
    }

public:
    // NOLINTBEGIN(*-non-private-member-variables-in-classes)

    window_interface window;
    input_system_interface input;

    // NOLINTEND(*-non-private-member-variables-in-classes)

private:
    class window* m_window;
    class input_system* m_input;

    friend class window;
};

} // namespace flow
