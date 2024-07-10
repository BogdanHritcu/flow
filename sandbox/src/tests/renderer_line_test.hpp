#pragma once

#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <flow/core/application.hpp>
#include <flow/core/logger.hpp>
#include <flow/graphics/opengl/commands.hpp>
#include <flow/graphics/orthographic_camera.hpp>
#include <flow/graphics/renderer/line_renderer.hpp>

class renderer_line_test final : public flow::application
{
public:
    void setup() final
    {
        engine.window.set_title("line_test");
        engine.window.set_size(640, 480); // NOLINT

        float aspect = static_cast<float>(engine.window.width()) / static_cast<float>(engine.window.height());
        m_camera.set_projection(-aspect, aspect, -1.0f, 1.0f, -10.0f, 10.0f);

        if (!m_renderer.create(1024))
        {
            engine.quit();
        }

        flow::gl::set_clear_color({ 0.0f, 0.0f, 0.0f, 1.0f });
    }

    void update(flow::duration /*dt*/) final
    {
        flow::gl::clear();

        glm::vec2 resolution = { static_cast<float>(engine.window.width()), static_cast<float>(engine.window.height()) };

        flow::gl::set_polygon_mode(flow::gl::polygon_mode::fill);

        glm::mat4 modelview = glm::translate(glm::mat4(1.0f), glm::vec3(-0.65f, 0.0f, 0.0f));
        modelview = glm::scale(modelview, glm::vec3(0.5f, 0.5f, 1.0f));

        constexpr auto loop_count = 6;

        m_renderer.begin_batch(m_camera.projection() * modelview, resolution, 10.0f);
        for (std::size_t i = 1; i <= loop_count; ++i)
        {
            m_renderer.submit(std::to_array<flow::line_renderer::vertex>({ { { 1.0f, 0.0f, 0.0f, 1.0f }, glm::vec2{ -0.2f, -0.2f } * static_cast<float>(i) },
                                                                           { { 0.0f, 1.0f, 0.0f, 1.0f }, glm::vec2{ -0.2f, 0.2f } * static_cast<float>(i) },
                                                                           { { 1.0f, 1.0f, 0.0f, 1.0f }, glm::vec2{ 0.2f, 0.2f } * static_cast<float>(i) },
                                                                           { { 0.0f, 0.0f, 1.0f, 1.0f }, glm::vec2{ 0.2f, -0.2f } * static_cast<float>(i) } }),
                              flow::line_strip_mode::loop);
        }
        m_renderer.end_batch();

        flow::gl::set_polygon_mode(flow::gl::polygon_mode::line);

        modelview = glm::translate(glm::mat4(1.0f), glm::vec3(0.65f, 0.0f, 0.0f));
        modelview = glm::scale(modelview, glm::vec3(0.5f, 0.5f, 1.0f));

        m_renderer.begin_batch(m_camera.projection() * modelview, resolution, 10.0f);
        for (std::size_t i = 1; i <= loop_count; ++i)
        {
            m_renderer.submit(std::array{ glm::vec2{ -0.2f, -0.2f } * static_cast<float>(i),
                                          glm::vec2{ -0.2f, 0.2f } * static_cast<float>(i),
                                          glm::vec2{ 0.2f, 0.2f } * static_cast<float>(i),
                                          glm::vec2{ 0.2f, -0.2f } * static_cast<float>(i) },
                              flow::line_strip_mode::loop);
        }
        m_renderer.end_batch();
    }

private:
    flow::orthographic_camera m_camera;
    flow::line_renderer m_renderer;
};
