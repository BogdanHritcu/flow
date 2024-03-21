#include "../../include/flow/core/window.hpp"

#include <string_view>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../../include/flow/core/assertion.hpp"
#include "../../include/flow/core/logger.hpp"
#include "../../include/flow/input/input_event.hpp"

namespace flow {
namespace detail {

    void destroy(GLFWwindow* ptr) noexcept
    {
        glfwDestroyWindow(ptr);
    }

    void terminate() noexcept
    {
        glfwTerminate();
    }

} // namespace detail

namespace gl {

    [[nodiscard]] constexpr std::string_view debug_message_source_name(GLenum value) noexcept;
    [[nodiscard]] constexpr std::string_view debug_message_type_name(GLenum value) noexcept;
    [[nodiscard]] constexpr std::string_view debug_message_severity_name(GLenum value) noexcept;
    void GLAPIENTRY error_callback(GLenum source_id,
                                   GLenum type_id,
                                   GLuint id,
                                   GLenum severity_id,
                                   GLsizei msg_length,
                                   const GLchar* msg,
                                   const void* user_param);

} // namespace gl

void glfw_error_callback(int error, const char* description)
{
    FLOW_LOG_ERROR("error ({}): {}", error, description);
}

window::window() noexcept
    : m_handle{}
    , m_window_data{ this }
{}

window::window(size_type width, size_type height, const std::string& title) noexcept
    : window()
{
    create(width, height, title);
}

bool window::create(size_type width, size_type height, const std::string& title) noexcept
{
    if (!glfwInit())
    {
        return false;
    }

    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    m_handle.reset(glfwCreateWindow(static_cast<int>(width),
                                    static_cast<int>(height),
                                    title.c_str(),
                                    nullptr,
                                    nullptr));

    if (m_handle)
    {
        glfwMakeContextCurrent(m_handle.get());
        glfwSetWindowUserPointer(m_handle.get(), &m_window_data);

        if (!gladLoadGL(glfwGetProcAddress))
        {
            return false;
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl::error_callback, nullptr);
    }

    return m_handle != nullptr;
}

void window::close() const noexcept
{
    glfwSetWindowShouldClose(m_handle.get(), 1);
}

void window::swap_buffers() const noexcept
{
    glfwSwapBuffers(m_handle.get());
}

void window::poll_events() const noexcept
{
    glfwPollEvents();
}

void window::set_vsync(bool value) const noexcept
{
    glfwSwapInterval(static_cast<int>(value)); // TODO: make this configurable
}

void window::set_size(size_type width, size_type height) const noexcept
{
    glfwSetWindowSize(m_handle.get(), static_cast<int>(width), static_cast<int>(height));
}

void window::set_title(const std::string& title) const noexcept
{
    glfwSetWindowTitle(m_handle.get(), title.c_str());
}

void window::set_user_pointer(void* ptr) noexcept
{
    m_window_data.user_ptr = ptr;
}

bool window::is_open() const noexcept
{
    return !glfwWindowShouldClose(m_handle.get());
}

window::size_type window::width() const noexcept
{
    int width;
    glfwGetWindowSize(m_handle.get(), &width, nullptr);
    return static_cast<size_type>(width);
}

window::size_type window::height() const noexcept
{
    int height;
    glfwGetWindowSize(m_handle.get(), nullptr, &height);
    return static_cast<size_type>(height);
}

void* window::get_user_pointer() const noexcept
{
    return m_window_data.user_ptr;
}

window::input_callback_type window::set_key_callback(input_callback_type callback) noexcept
{
    input_callback_type old_callback{ m_window_data.key_callback };
    m_window_data.key_callback = callback;

    glfwSetKeyCallback(m_handle.get(),
                       [](GLFWwindow* glfw_window, int code, int scancode, int action, int mod)
                       {
                           if (code == GLFW_KEY_UNKNOWN)
                           {
                               return;
                           }

                           window_data* data = static_cast<window_data*>(glfwGetWindowUserPointer(glfw_window));

                           input_binding binding{
                               static_cast<key>(code),
                               static_cast<input_modifier>(mod),
                               static_cast<input_action>(action)
                           };

                           data->key_callback(*data->window, input_event{ binding });
                       });

    return old_callback;
}

window::input_callback_type window::set_mouse_button_callback(input_callback_type callback) noexcept
{
    input_callback_type old_callback{ m_window_data.mouse_button_callback };
    m_window_data.mouse_button_callback = callback;

    glfwSetMouseButtonCallback(m_handle.get(),
                               [](GLFWwindow* glfw_window, int code, int action, int mod)
                               {
                                   window_data* data = static_cast<window_data*>(glfwGetWindowUserPointer(glfw_window));

                                   input_binding binding{
                                       static_cast<mouse_button>(code),
                                       static_cast<input_modifier>(mod),
                                       static_cast<input_action>(action)
                                   };

                                   data->key_callback(*data->window, input_event{ binding });
                               });

    return old_callback;
}

window::resize_callback_type window::set_window_resize_callback(resize_callback_type callback) noexcept
{
    resize_callback_type old_callback{ m_window_data.window_resize_callback };
    m_window_data.window_resize_callback = callback;

    glfwSetWindowSizeCallback(m_handle.get(),
                              [](GLFWwindow* glfw_window, int width, int height)
                              {
                                  window_data* data = static_cast<window_data*>(glfwGetWindowUserPointer(glfw_window));

                                  data->window_resize_callback(*data->window, static_cast<size_type>(width), static_cast<size_type>(height));
                              });

    return old_callback;
}

window::resize_callback_type window::set_framebuffer_resize_callback(resize_callback_type callback) noexcept
{
    resize_callback_type old_callback{ m_window_data.framebuffer_resize_callback };
    m_window_data.framebuffer_resize_callback = callback;

    glfwSetFramebufferSizeCallback(m_handle.get(),
                                   [](GLFWwindow* glfw_window, int width, int height)
                                   {
                                       window_data* data = static_cast<window_data*>(glfwGetWindowUserPointer(glfw_window));

                                       data->framebuffer_resize_callback(*data->window, static_cast<size_type>(width), static_cast<size_type>(height));
                                   });

    return old_callback;
}

namespace gl {

    [[nodiscard]] constexpr std::string_view debug_message_source_name(GLenum value) noexcept
    {
        switch (value)
        {
        case GL_DEBUG_SOURCE_API:
            return "createGL API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "window system";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "shader compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "third party";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "application";
        default:
            return "other";
        }
    }

    [[nodiscard]] constexpr std::string_view debug_message_type_name(GLenum value) noexcept
    {
        switch (value)
        {
        case GL_DEBUG_TYPE_ERROR:
            return "error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "deprecated behavior";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "undefined behavior";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "portability";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "performance";
        case GL_DEBUG_TYPE_MARKER:
            return "marker";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            "push group";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "pop group";
        default:
            return "other";
        }
    }

    [[nodiscard]] constexpr std::string_view debug_message_severity_name(GLenum value) noexcept
    {
        switch (value)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            return "high";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "medium";
        case GL_DEBUG_SEVERITY_LOW:
            return "low";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "notification";
        default:
            return "other";
        }
    }

    void GLAPIENTRY error_callback(GLenum source_id,
                                   GLenum type_id,
                                   GLuint id,
                                   GLenum severity_id,
                                   GLsizei msg_length,
                                   const GLchar* msg,
                                   const void* user_param)
    {
        std::string_view source = debug_message_source_name(source_id);
        std::string_view type = debug_message_type_name(type_id);
        std::string_view severity = debug_message_severity_name(severity_id);
        std::string_view message(msg, msg_length);

        switch (type_id)
        {
        case GL_DEBUG_TYPE_ERROR:
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            FLOW_LOG_ERROR("[{}] ({}): {}", source, type, message);
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        case GL_DEBUG_TYPE_PORTABILITY:
            FLOW_LOG_WARN("[{}] ({}): {}", source, type, message);
            break;
        case GL_DEBUG_TYPE_MARKER:
        case GL_DEBUG_TYPE_PUSH_GROUP:
        case GL_DEBUG_TYPE_POP_GROUP:
            FLOW_LOG_INFO("[{}]: {}", source, message);
            break;
        default:
            FLOW_LOG_TRACE("[{}] ({}) ({}): {}", source, type, severity, message);
            break;
        }
    }

} // namespace gl
} // namespace flow
