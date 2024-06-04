#include "../../include/flow/core/window.hpp"
#include "include/flow/core/engine_interface.hpp"

#include <string_view>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../../include/flow/core/logger.hpp"
#include "../../include/flow/input/binding.hpp"
#include "../../include/flow/input/binding_enums.hpp"

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

#ifdef FLOW_DEBUG
void glfw_error_callback(int error, const char* description);

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
#endif

window::window(engine_interface* engine, const settings& settings) noexcept
    : window()
{
    create(engine, settings);
}

bool window::create(engine_interface* engine, const settings& settings) noexcept
{
    m_window_data.engine = engine;
    m_settings = settings;

    if (!glfwInit())
    {
        return false;
    }

#ifdef FLOW_DEBUG
    glfwSetErrorCallback(glfw_error_callback);
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // NOLINT
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // NOLINT

    m_handle.reset(glfwCreateWindow(static_cast<int>(m_settings.width),
                                    static_cast<int>(m_settings.height),
                                    m_settings.title.c_str(),
                                    nullptr,
                                    nullptr));

    if (m_handle)
    {
        glfwMakeContextCurrent(m_handle.get());

        glfwSetWindowUserPointer(m_handle.get(), &m_window_data);
        glfwSwapInterval(static_cast<int>(m_settings.vsync));

        if (!gladLoadGL(glfwGetProcAddress))
        {
            return false;
        }

        set_callbacks();

#ifdef FLOW_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl::error_callback, nullptr);
#endif
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

void window::set_title(std::string_view title) noexcept
{
    m_settings.title = title;
    glfwSetWindowTitle(m_handle.get(), m_settings.title.c_str());
}

void window::set_size(size_type width, size_type height) noexcept // NOLINT
{
    m_settings.width = width;
    m_settings.height = height;
    glfwSetWindowSize(m_handle.get(),
                      static_cast<int>(m_settings.width),
                      static_cast<int>(m_settings.height));
}

void window::set_vsync(bool value) noexcept
{
    m_settings.vsync = value;
    glfwSwapInterval(static_cast<int>(m_settings.vsync));
}

bool window::is_open() const noexcept
{
    return !glfwWindowShouldClose(m_handle.get());
}

void window::set_callbacks() noexcept
{
    set_key_callback();
    set_mouse_button_callback();
    set_framebuffer_resize_callback();
}

void window::set_key_callback() noexcept
{
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    auto adaptor_callback = [](GLFWwindow* glfw_window, int code, int /*scancode*/, int action, int mod) -> void
    {
        if (code < 0 || code == GLFW_KEY_UNKNOWN)
        {
            return;
        }

        auto* data = static_cast<window_data*>(glfwGetWindowUserPointer(glfw_window));

        if (!(data && data->engine && data->engine->m_input))
        {
            return;
        }

        binding bind = make_binding(detail::add_code_flag<key_code>(
                                        static_cast<detail::binding_code_underlying_type>(code)),
                                    static_cast<binding_action_code>(action),
                                    static_cast<binding_modifier_code>(mod));

        data->engine->m_input->invoke_binding_callbacks(bind, *data->engine);
    };

    glfwSetKeyCallback(m_handle.get(), adaptor_callback);
}

void window::set_mouse_button_callback() noexcept
{
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    auto adaptor_callback = [](GLFWwindow* glfw_window, int code, int action, int mod) -> void
    {
        if (code < 0)
        {
            return;
        }

        auto* data = static_cast<window_data*>(glfwGetWindowUserPointer(glfw_window));

        if (!(data && data->engine && data->engine->m_input))
        {
            return;
        }

        binding bind = make_binding(detail::add_code_flag<mbtn_code>(
                                        static_cast<detail::binding_code_underlying_type>(code)),
                                    static_cast<binding_action_code>(action),
                                    static_cast<binding_modifier_code>(mod));

        data->engine->m_input->invoke_binding_callbacks(bind, *data->engine);
    };

    glfwSetMouseButtonCallback(m_handle.get(), adaptor_callback);
}

void window::set_framebuffer_resize_callback() noexcept
{
    auto adaptor_callback = [](GLFWwindow* /*glfw_window*/, int width, int height) -> void
    {
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    };

    glfwSetFramebufferSizeCallback(m_handle.get(), adaptor_callback);
}

#ifdef FLOW_DEBUG
void glfw_error_callback(int error, const char* description)
{
    FLOW_LOG_ERROR("error ({}): {}", error, description);
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
            return "push group";
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

    // NOLINTBEGIN(bugprone-easily-swappable-parameters)
    void GLAPIENTRY error_callback(GLenum source_id,
                                   GLenum type_id,
                                   GLuint /*id*/,
                                   GLenum severity_id,
                                   GLsizei msg_length,
                                   const GLchar* msg,
                                   const void* /*user_param*/)
    // NOLINTEND(bugprone-easily-swappable-parameters)
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
#endif

} // namespace flow
