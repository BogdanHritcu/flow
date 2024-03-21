#pragma once

#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <glad/gl.h>

#include "../../utility/unique_handle.hpp"
#include "../../core/logger.hpp" // temp

namespace flow {
namespace gl {

    namespace fs = std::filesystem;

    enum class shader_type : GLenum
    {
        // clang-format off

        vertex   = GL_VERTEX_SHADER,
        fragment = GL_FRAGMENT_SHADER,
        geometry = GL_GEOMETRY_SHADER,
        compute  = GL_COMPUTE_SHADER

        // clang-format on
    };

    struct shader_config
    {
        fs::path path;
        gl::shader_type type;
    };

    class shader
    {
    public:
        using id_type = GLuint;
        // clang-format off
        using deleter_type = decltype([](id_type id) { glDeleteShader(id); });
        // clang-format on
        using handle_type = unique_handle<id_type, deleter_type>;

    public:
        constexpr shader() noexcept
            : m_handle{}
        {}

        bool create(shader_type type) noexcept
        {
            id_type id = glCreateShader(static_cast<GLenum>(type));
            m_handle.reset(id);

            return id != 0;
        }

        bool create(const shader_config& config)
        {
            return create(config.type) && from_file(config.path);
        }

        bool compile() const
        {
            glCompileShader(m_handle.get());

            return get_compile_status();
        }

        [[nodiscard]] bool get_compile_status() const noexcept
        {
            GLint success = GL_FALSE;
            glGetShaderiv(m_handle.get(), GL_COMPILE_STATUS, &success);

            return success != GL_FALSE;
        }

        [[nodiscard]] std::string get_info_log() const
        {
            GLint logLength = 0;
            glGetShaderiv(m_handle.get(), GL_INFO_LOG_LENGTH, &logLength);

            std::string logMessage(logLength, '\0');

            glGetShaderInfoLog(m_handle.get(), logLength, &logLength, logMessage.data());

            return logMessage;
        }

        bool from_string(std::string_view source) const noexcept
        {
            // we have to be explicit about the size of the source
            // because std::string_view may not be null terminated
            const char* data = source.data();
            GLint sizes[] = { static_cast<GLint>(source.size()) };
            glShaderSource(m_handle.get(), 1, &data, sizes);

            return true;
        }

        bool from_file(const fs::path& path) const
        {
            if (!fs::exists(path) || !fs::is_regular_file(path))
            {
                return false;
            }

            std::fstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

            std::size_t size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::string source;
            source.resize(size);
            file.read(source.data(), source.size());

            return from_string(source);
        }

        [[nodiscard]] constexpr id_type id() const noexcept
        {
            return m_handle.get();
        }

    private:
        handle_type m_handle;
    };

    class shader_program
    {
    public:
        using id_type = GLuint;
        // clang-format off
        using deleter_type = decltype([](id_type id) { glDeleteProgram(id); });
        // clang-format on
        using handle_type = unique_handle<id_type, deleter_type>;

    public:
        constexpr shader_program() noexcept
            : m_handle{}
        {}

        bool create() noexcept
        {
            id_type id = glCreateProgram();
            m_handle.reset(id);

            return id != 0;
        }

        bool link(std::span<shader> shaders) const
        {
            attach(shaders);

            glLinkProgram(m_handle.get());

            detach(shaders);

            return get_link_status();
        }

        bool link(std::initializer_list<shader> shaders) const
        {
            attach(shaders);

            glLinkProgram(m_handle.get());

            detach(shaders);

            return get_link_status();
        }

        bool link(std::span<shader_config> shader_configs) const
        {
            std::vector<shader> shaders;

            for (auto& config : shader_configs)
            {
                shader shader;
                if (!(shader.create(config) && shader.compile()))
                {
                    FLOW_LOG_ERROR(shader.get_info_log()); // temp
                    return false;
                }
                shaders.push_back(std::move(shader));
            }

            attach(shaders);

            glLinkProgram(m_handle.get());

            detach(shaders);

            return get_link_status();
        }

        bool link(std::initializer_list<shader_config> shader_configs) const
        {
            std::vector<shader> shaders;

            for (auto& config : shader_configs)
            {
                shader shader;
                if (!(shader.create(config) && shader.compile()))
                {
                    FLOW_LOG_ERROR(shader.get_info_log()); // temp
                    return false;
                }
                shaders.push_back(std::move(shader));
            }

            attach(shaders);

            glLinkProgram(m_handle.get());

            detach(shaders);

            return get_link_status();
        }

        void attach(std::span<shader> shaders) const noexcept
        {
            for (auto& shader : shaders)
            {
                glAttachShader(m_handle.get(), shader.id());
            }
        }

        void attach(std::initializer_list<shader> shaders) const noexcept
        {
            for (auto& shader : shaders)
            {
                glAttachShader(m_handle.get(), shader.id());
            }
        }

        void detach(std::span<shader> shaders) const noexcept
        {
            for (auto& shader : shaders)
            {
                glDetachShader(m_handle.get(), shader.id());
            }
        }

        void detach(std::initializer_list<shader> shaders) const noexcept
        {
            for (auto& shader : shaders)
            {
                glDetachShader(m_handle.get(), shader.id());
            }
        }

        [[nodiscard]] bool get_link_status() const noexcept
        {
            GLint success = GL_FALSE;
            glGetProgramiv(m_handle.get(), GL_LINK_STATUS, &success);

            return success != GL_FALSE;
        }

        [[nodiscard]] std::string get_info_log() const
        {
            GLint logLength = 0;
            glGetProgramiv(m_handle.get(), GL_INFO_LOG_LENGTH, &logLength);

            std::string logMessage(logLength, '\0');

            glGetProgramInfoLog(m_handle.get(), logLength, &logLength, logMessage.data());

            return logMessage;
        }

        void use() const noexcept
        {
            glUseProgram(m_handle.get());
        }

        [[nodiscard]] constexpr id_type id() const noexcept
        {
            return m_handle.get();
        }

    private:
        handle_type m_handle;
    };

} // namespace gl
} // namespace flow
