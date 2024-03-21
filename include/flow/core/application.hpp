#pragma once

#include <vector>

#include <entt/container/dense_map.hpp>

#include "../input/input_code.hpp"
#include "../input/input_context.hpp"
#include "window.hpp"

namespace flow {
namespace detail {

    int entry(int argc, char** argv);

}

class input
{
public:
    input()
        : m_input_states{}
        , m_contexts{}
        , m_pressed_keys_count{}
        , m_pressed_mouse_buttons_count{}
    {}

    template<typename T>
    [[nodiscard]] bool is_pressed(T code) const noexcept
    {
        return m_input_states[input_code{ code }] != input_action::Release;
    }

    template<typename T>
    [[nodiscard]] bool any_pressed() const noexcept
    {
        return pressed_count<T>() != 0;
    }

    [[nodiscard]] bool any_pressed() const noexcept
    {
        return pressed_count() != 0;
    }

    template<typename T>
    [[nodiscard]] std::size_t pressed_count() const noexcept
    {
        return get_pressed_counter<T>();
    }

    [[nodiscard]] std::size_t pressed_count() const noexcept
    {
        return m_pressed_keys_count + m_pressed_mouse_buttons_count;
    }

    void add_context(const input_context& context)
    {
        m_contexts.push_back(context);
    }

    [[nodiscard]] input_context& getContext(std::size_t index) noexcept
    {
        return m_contexts[index];
    }

private:
    // allow only specializations
    template<typename T>
    std::size_t& get_pressed_counter() noexcept = delete;

    template<>
    std::size_t& get_pressed_counter<key>() noexcept
    {
        return m_pressed_keys_count;
    }

    template<>
    std::size_t& get_pressed_counter<mouse_button>() noexcept
    {
        return m_pressed_mouse_buttons_count;
    }

private:
    std::vector<input_context> m_contexts;
    entt::dense_map<input_code, input_action> m_input_states;
    std::size_t m_pressed_keys_count;
    std::size_t m_pressed_mouse_buttons_count;

    friend class application;
};

class application
{
public:
    using size_type = std::size_t;

public:
    application();
    application(size_type width, size_type height, const std::string& title);
    application(const application& other) = delete;
    application& operator=(const application& other) = delete;
    application(application&& other) = delete;
    application& operator=(application&& other) = delete;
    virtual ~application();

    virtual void start();
    virtual void update(float dt);
    void quit() const noexcept;

    void set_window_size(size_type width, size_type height) const noexcept;
    void set_window_title(const std::string& title) const noexcept;
    void set_window_vsync(bool value) const noexcept;

private:
    void run();

    template<typename T>
    static void input_callback(window& window, const input_event& event)
    {
        input.m_input_states[event.binding.code] = event.binding.action;
        input.get_pressed_counter<T>() += event.binding.action == input_action::press ? 1 : -1;

        for (auto& context : input.m_contexts)
        {
            if (context.is_active())
            {
                bool handled1{ context.handle(event) };
                bool handled2{ context.handle_any<T>(event) };
                bool handled3{ context.handle_any<key, mouse_button>(event) };

                if (handled1 || handled2 || handled3)
                {
                    break;
                }
            }
        }
    }

public:
    static input input;

private:
    window m_window;

    friend int ::flow::detail::entry(int argc, char** argv);
};

} // namespace flow
