#pragma once

#include <cstdint>
#include <memory>
#include <string>

struct GLFWwindow;

namespace flow {

struct input_event;

namespace detail {

    void destroy(GLFWwindow* ptr) noexcept;
    void terminate() noexcept;

} // namespace detail

class window
{
public:
    using value_type = GLFWwindow;
    using pointer_type = value_type*;
    using size_type = std::size_t;
    // clang-format off
    using deleter_type = decltype([](pointer_type ptr) { detail::destroy(ptr); detail::terminate(); });
    // clang-format on
    using handle_type = std::unique_ptr<value_type, deleter_type>;

    using input_callback_type = void (*)(window&, const input_event&);
    using resize_callback_type = void (*)(window&, size_type, size_type);

public:
    window() noexcept;
    window(size_type width, size_type height, const std::string& title) noexcept;

    bool create(size_type width, size_type height, const std::string& title) noexcept;
    void close() const noexcept;
    void swap_buffers() const noexcept;
    void poll_events() const noexcept;
    void set_vsync(bool value) const noexcept;

    void set_size(size_type width, size_type height) const noexcept;
    void set_title(const std::string& title) const noexcept;

    void set_user_pointer(void* ptr) noexcept;
    input_callback_type set_key_callback(input_callback_type callback) noexcept;
    input_callback_type set_mouse_button_callback(input_callback_type callback) noexcept;
    resize_callback_type set_window_resize_callback(resize_callback_type callback) noexcept;
    resize_callback_type set_framebuffer_resize_callback(resize_callback_type callback) noexcept;

    [[nodiscard]] bool is_open() const noexcept;
    [[nodiscard]] size_type width() const noexcept;
    [[nodiscard]] size_type height() const noexcept;
    [[nodiscard]] void* get_user_pointer() const noexcept;

private:
    struct window_data
    {
        window* window;
        void* user_ptr;
        input_callback_type key_callback;
        input_callback_type mouse_button_callback;
        resize_callback_type window_resize_callback;
        resize_callback_type framebuffer_resize_callback;
    };

private:
    handle_type m_handle;
    window_data m_window_data;
};

} // namespace flow
