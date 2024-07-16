#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <flow/core/application.hpp>
#include <flow/graphics/opengl/commands.hpp>
#include <flow/graphics/orthographic_camera.hpp>
#include <flow/graphics/renderer/rectangle_renderer.hpp>

class rectangle_renderer_test final : public flow::application
{
public:
    void setup() override
    {
        engine.window.set_title("rectangle_test");
        engine.window.set_size(640, 480); // NOLINT

        m_camera.set_projection(0.0f, 640.0f, 480.0f, 0.0f, -10.0f, 10.0f);

        if (!m_renderer.create(1024))
        {
            engine.quit();
        }

        flow::gl::set_clear_color({ 0.0f, 0.0f, 0.0f, 1.0f });
    }

    void update(flow::duration /*dt*/) override
    {
        flow::gl::clear();

        auto view1 = glm::translate(glm::mat4(1.0f), { 20.0f, 40.0f, 0.0f });
        view1 = glm::scale(view1, { 1.5f, 1.5f, 0.0f });
        auto view2 = glm::translate(glm::mat4(1.0f), { 320.0f, 40.0f, 0.0f });
        view2 = glm::scale(view2, { 1.5f, 1.5f, 0.0f });

        m_renderer.begin_batch(m_camera.projection() * view1);
        m_renderer.submit({ 0.0f, 0.0f },
                          { 64.0f, 64.0f },
                          { 1.0f, 0.0f, 0.0f, 1.0f });
        m_renderer.submit({ 64.0f, 64.0f },
                          { 64.0f, 64.0f },
                          { 0.0f, 1.0f, 0.0f, 1.0f });
        m_renderer.submit({ 128.0f, 128.0f },
                          { 64.0f, 64.0f },
                          { 0.0f, 0.0f, 1.0f, 1.0f });
        m_renderer.submit({ 64.0f, 64.0f },
                          { 32.0f, 32.0f },
                          { 0.5f, 0.5f },
                          { 1.0f, 1.0f, 0.0f, 1.0f });
        m_renderer.submit({ 128.0f, 128.0f },
                          { 32.0f, 32.0f },
                          { 0.5f, 0.5f },
                          { 0.0f, 1.0f, 1.0f, 1.0f });
        flow::gl::set_polygon_mode(flow::gl::polygon_mode::fill);
        m_renderer.end_batch(false);

        m_renderer.set_view_projection(m_camera.projection() * view2);
        flow::gl::set_polygon_mode(flow::gl::polygon_mode::line);
        m_renderer.end_batch();
    }

private:
    flow::orthographic_camera m_camera;
    flow::rectangle_renderer m_renderer;
};
