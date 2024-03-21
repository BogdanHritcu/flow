#pragma once

#include <array>
#include <cmath>
#include <filesystem>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/type_aligned.hpp>

#include <flow/core/application.hpp>
#include <flow/graphics/orthographic_camera.hpp>
#include <flow/graphics/renderer/instance_renderer.hpp>
#include <flow/graphics/texture/texture_array.hpp>
#include <flow/utility/noise.hpp>
#include <flow/utility/random.hpp>
#include <flow/utility/time.hpp>

class texture_quad_test final : public flow::application
{
private:
    struct rectangle
    {
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 origin;
        float rotation;
        glm::uint tex_index;
    };

    struct instance_state
    {
        glm::aligned_mat4 transform;
        glm::aligned_vec4 color;
        glm::aligned_vec2 tex_coords[4];
        glm::aligned_vec2 origin;
        float tex_layer;
    };

public:
    void start() final
    {
        namespace fs = std::filesystem;

        // load textures

        flow::texture_array_config texture_config = {
            .width = 16,
            .height = 16,
            .format = flow::image_format::rgba
        };

        std::array paths = {
            fs::path("data/textures/terrain/dirt.png"),
            fs::path("data/textures/terrain/stone.png"),
        };

        flow::fixed_image_atlas atlas;
        std::size_t loaded = atlas.create(16, 16, flow::image_format::rgba, paths);

        if (loaded != paths.size())
        {
            FLOW_LOG_WARN("loaded {}/{} textures", loaded, paths.size());
        }

        m_texture_array.load(atlas);

        // load shaders

        flow::gl::shader_config shaders[] = {
            {
                .path = "data/shaders/default_shader.vrt",
                .type = flow::gl::shader_type::vertex,
            },
            {
                .path = "data/shaders/default_shader.frg",
                .type = flow::gl::shader_type::fragment,
            },
        };

        if (!(m_shader.create() && m_shader.link(shaders)))
        {
            FLOW_LOG_ERROR(m_shader.get_info_log());
        }

        // setup renderer

        glm::vec2 positions[] = {
            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f }
        };

        glm::uint elements[] = { 0, 1, 2, 2, 3, 0 }; // triangles

        flow::gl::attribute_config attributes[] = {
            {
                .format = {
                    .offset = 0,
                    .count = 2,
                    .type = flow::gl::type_value::gl_float },
                .index = 0,
                .normalize = false,
            }
        };

        flow::renderer_config<glm::vec2, glm::uint> renderer_config{};
        renderer_config.capacity = 100000;
        renderer_config.buffer_count = 3;
        renderer_config.vertices = positions;
        renderer_config.attributes = attributes;
        renderer_config.indices = elements;
        renderer_config.render_mode = flow::render_mode::triangles;

        if (!m_renderer.create(renderer_config))
        {
            FLOW_LOG_ERROR("failed to create renderer");
        }

        // setup camera

        m_camera.set_projection(0.0f, m_state.width, m_state.height, 0.0f, -200.0f, 200.0f);
        m_camera.set_position(0.0f, 0.0f, 200.0f);

        // setup rectangles

        const std::size_t width_rect_count = static_cast<std::size_t>(m_state.width / m_state.rect_width);
        const std::size_t height_rect_count = static_cast<std::size_t>(m_state.height / m_state.rect_height);

        m_state.rectangles.resize(width_rect_count * height_rect_count);

        for (std::size_t i = 0; i < m_state.rectangles.size(); ++i)
        {
            std::size_t x = i % width_rect_count;
            std::size_t y = i / width_rect_count;

            m_state.rectangles[i].position = { x * m_state.rect_width, y * m_state.rect_height };
            m_state.rectangles[i].size = { m_state.rect_width, m_state.rect_height };
            m_state.rectangles[i].rotation = 0.0f;
            m_state.rectangles[i].origin = { 0.0f, 0.0f };
            m_state.rectangles[i].tex_index = m_state.random_generator.uniform<glm::uint>(0, 1);
        }

        // setup window

        for (auto str = glGetString(GL_VERSION); str && *str != '\0'; ++str)
        {
            m_state.gl_version.push_back(static_cast<char>(*str));
        }

        for (auto str = glGetString(GL_SHADING_LANGUAGE_VERSION); str && *str != '\0'; ++str)
        {
            m_state.gl_shading_version.push_back(static_cast<char>(*str));
        }

        set_window_size(static_cast<std::size_t>(m_state.width), static_cast<std::size_t>(m_state.height));
        set_window_title(std::format("{} {} ({:.2f}fps {:.6f}s)", m_state.gl_version, m_state.gl_shading_version, 0.0, 0.0));
        set_window_vsync(false);

        // other

        flow::gl::set_clear_color({ 0.2f, 0.2f, 0.25f, 1.0f });

        m_state.stopwatch.reset();

        FLOW_LOG_INFO("Rendering {} textured rectangles", m_state.rectangles.size());
    }

    void update(float dt) final
    {
        double elapsed_time = m_state.stopwatch.elapsed().seconds();
        double frame_time = elapsed_time - m_state.last_elapsed_time;
        m_state.last_elapsed_time = elapsed_time;
        m_state.title_time_accumulator += frame_time;

        if (m_state.title_time_accumulator >= 1.0)
        {
            m_state.title_time_accumulator -= 1.0;
            set_window_title(std::format("{:s} {:s} ({:.2f}fps {:.6f}s)",
                                         m_state.gl_version,
                                         m_state.gl_shading_version,
                                         1.0 / frame_time,
                                         frame_time));
        }

        flow::gl::clear();

        auto mv_mat = m_camera.projection() * m_camera.view();

        m_renderer.begin_batch();

        auto modifier_generator = flow::noise_generator<2>{ m_state.rectangles.size() };

        for (auto i = 0; i < m_state.rectangles.size(); ++i)
        {
            float modifier = static_cast<float>(modifier_generator(elapsed_time * 0.05, 0.0));
            modifier *= modifier;
            modifier += 0.2f;

            float noise = m_state.noise_generator(m_state.rectangles[i].position / (modifier * 32.0f * m_state.rect_width));
            m_state.rectangles[i].tex_index = noise < 0.4f ? 0 : 1;
            m_renderer.submit(instance_factory(mv_mat, m_state.rectangles[i]));
        }

        m_shader.use();
        m_texture_array.bind();
        m_renderer.end_batch();
    }

private:
    static instance_state instance_factory(const glm::mat4& transform,
                                           const rectangle& rect,
                                           const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) noexcept
    {
        glm::mat4 new_transform = glm::translate(transform, glm::vec3(rect.position, 1.0f));
        new_transform = glm::rotate(new_transform, glm::radians(rect.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        new_transform = glm::scale(new_transform, glm::vec3(rect.size, 1.0f));

        static constexpr glm::vec2 tex_tl{ 0.0f, 1.0f };
        static constexpr glm::vec2 tex_bl{ 0.0f, 0.0f };
        static constexpr glm::vec2 tex_br{ 1.0f, 0.0f };
        static constexpr glm::vec2 tex_tr{ 1.0f, 1.0f };

        return {
            .transform = new_transform,
            .color = color,
            .tex_coords = { tex_tl, tex_bl, tex_br, tex_tr },
            .origin = rect.origin,
            .tex_layer = static_cast<float>(rect.tex_index)
        };
    }

private:
    flow::instance_renderer<instance_state> m_renderer;
    flow::texture_array m_texture_array;
    flow::gl::shader_program m_shader;
    flow::orthographic_camera m_camera;

    struct state
    {
        static constexpr float width = 1080.0f;
        static constexpr float height = 720.0f;
        static constexpr float rect_width = 4.0f;
        static constexpr float rect_height = 4.0f;

        std::vector<rectangle> rectangles{};

        flow::stopwatch stopwatch{};
        flow::random_generator random_generator{ std::random_device{}() };
        flow::noise_generator<2> noise_generator{ std::random_device{}() };

        double title_time_accumulator{};
        double last_elapsed_time{};
        std::string gl_version{};
        std::string gl_shading_version{};
    };

    state m_state;
};
