#pragma once

#include <flow/core/application.hpp>
#include <flow/core/logger.hpp>
#include <flow/graphics/orthographic_camera.hpp>
#include <flow/graphics/renderer/line_renderer.hpp>
#include <flow/graphics/renderer/rectangle_renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <flow/physics/dynamic_body.hpp>
#include <flow/physics/static_body.hpp>
#include <flow/physics/world.hpp>

constexpr float m_to_px = 32.0f;
constexpr float px_to_m = 1.0f / m_to_px;

constexpr std::uint32_t player_tag = 1;
constexpr std::uint32_t ground_tag = 2;

struct movement_data
{
    static constexpr std::uint32_t grounded_flag = 0x1;
    static constexpr std::uint32_t moving_left_flag = 0x2;
    static constexpr std::uint32_t moving_right_flag = 0x4;

    std::uint32_t flags{};

    glm::vec2 force_accumulator{};

    glm::vec2 curr_velocity{};
    glm::vec2 prev_velocity{};

    glm::vec2 curr_acceleration{};
    glm::vec2 prev_acceleration{};

    glm::vec2 ghost_position{};

    float time = 0.0f;

    float move_force = 40.0f;
    float jump_force = 800.0f;
};

struct movement_record
{
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 acceleration;
    float duration;
};

static constexpr auto initial_position = glm::vec2{ 256.0f, 96.0f } * px_to_m;

class physics_test final : public flow::application
{
public:
    void setup() override
    {
        engine.window.set_title("physics_test");
        engine.window.set_size(640, 480); // NOLINT

        if (!m_renderer.create(1024))
        {
            FLOW_LOG_ERROR("failed to create renderer");
            engine.quit();
        }

        if (!m_line_renderer.create(1024))
        {
            FLOW_LOG_ERROR("failed to create line renderer");
            engine.quit();
        }

        m_camera.set_projection(0.0f,
                                static_cast<float>(engine.window.width()),
                                static_cast<float>(engine.window.height()),
                                0.0f,
                                -200.0f,
                                200.0f);
        m_camera.set_position(0.0f, 0.0f, 200.0f);

        engine.input.register_binding_context("movement");

        engine.input.register_binding_callback("set_move_left",
                                               [&](flow::engine_interface)
                                               {
                                                   m_movement_data.flags |= movement_data::moving_left_flag;
                                               });

        engine.input.register_binding_callback("clear_move_left",
                                               [&](flow::engine_interface)
                                               {
                                                   m_movement_data.flags &= ~movement_data::moving_left_flag;
                                               });

        engine.input.register_binding_callback("set_move_right",
                                               [&](flow::engine_interface)
                                               {
                                                   m_movement_data.flags |= movement_data::moving_right_flag;
                                               });

        engine.input.register_binding_callback("clear_move_right",
                                               [&](flow::engine_interface)
                                               {
                                                   m_movement_data.flags &= ~movement_data::moving_right_flag;
                                               });

        engine.input.register_binding_callback("jump",
                                               [&](flow::engine_interface)
                                               {
                                                   if (m_movement_data.flags & movement_data::grounded_flag)
                                                   {
                                                       m_movement_data.force_accumulator += glm::vec2{ 0.0f, -m_movement_data.jump_force };
                                                   }
                                               });

        engine.input.register_binding_callback("toggle_reverse_time",
                                               [&](flow::engine_interface)
                                               {
                                                   m_reversed_time = !m_reversed_time;
                                               });

        engine.input.register_binding({ engine.key.space, engine.action.press },
                                      "jump",
                                      "movement");

        engine.input.register_binding({ engine.key.a, engine.action.press },
                                      "set_move_left",
                                      "movement");

        engine.input.register_binding({ engine.key.a, engine.action.release },
                                      "clear_move_left",
                                      "movement");

        engine.input.register_binding({ engine.key.d, engine.action.press },
                                      "set_move_right",
                                      "movement");

        engine.input.register_binding({ engine.key.d, engine.action.release },
                                      "clear_move_right",
                                      "movement");

        engine.input.register_binding({ engine.key.r, engine.action.press },
                                      "toggle_reverse_time",
                                      "movement");

        engine.input.push_binding_context("movement");
    }

    void start() override
    {
        m_world.create({ 0.0f, 0.0f });

        m_world.set_contact_listener(
                [&](const flow::contact<glm::vec2>& con)
                {
                    bool on_ground = con.body_a_ptr->tag() == player_tag && con.body_b_ptr->tag() == ground_tag && con.normal.y == 1.0f
                            || con.body_a_ptr->tag() == ground_tag && con.body_b_ptr->tag() == player_tag && con.normal.y == -1.0f;

                    m_movement_data.flags |= (movement_data::grounded_flag * static_cast<std::uint32_t>(on_ground));
                },
                [&](const flow::contact<glm::vec2>& con)
                {
                    bool on_ground = con.body_a_ptr->tag() == player_tag && con.body_b_ptr->tag() == ground_tag && con.normal.y == 1.0f
                            || con.body_a_ptr->tag() == ground_tag && con.body_b_ptr->tag() == player_tag && con.normal.y == -1.0f;

                    m_movement_data.flags &= ~(movement_data::grounded_flag * static_cast<std::uint32_t>(on_ground));
                });

        m_body.create(m_world,
                      initial_position,
                      glm::vec2{ 32.0f, 32.0f } * px_to_m,
                      1.0f,
                      1.0f,
                      player_tag);

        m_movement_data.ghost_position = m_body.position();

        m_static_bodies.emplace_back(m_world,
                                     glm::vec2{ 320.0f, 472.0f } * px_to_m,
                                     glm::vec2{ 320.0f, 8.0f } * px_to_m,
                                     ground_tag);

        m_static_bodies.emplace_back(m_world,
                                     glm::vec2{ 320.0f, 8.0f } * px_to_m,
                                     glm::vec2{ 320.0f, 8.0f } * px_to_m,
                                     ground_tag);

        m_static_bodies.emplace_back(m_world,
                                     glm::vec2{ 8.0f, 240.0f } * px_to_m,
                                     glm::vec2{ 8.0f, 223.0f } * px_to_m,
                                     ground_tag);

        m_static_bodies.emplace_back(m_world,
                                     glm::vec2{ 632.0f, 240.0f } * px_to_m,
                                     glm::vec2{ 8.0f, 223.0f } * px_to_m,
                                     ground_tag);
    }

    void fixed_update(flow::duration dt) override
    {
        auto dtf = flow::as_seconds<float>(dt);

        if (!m_reversed_time)
        {
            // gravity
            m_movement_data.force_accumulator += glm::vec2{ 0.0f, 10.0f * m_body.mass() };

            if ((m_movement_data.flags & movement_data::grounded_flag) && (m_movement_data.flags & movement_data::moving_left_flag))
            {
                m_movement_data.force_accumulator += glm::vec2{ -m_movement_data.move_force, 0.0f };
            }

            if ((m_movement_data.flags & movement_data::grounded_flag) && (m_movement_data.flags & movement_data::moving_right_flag))
            {
                m_movement_data.force_accumulator += glm::vec2{ m_movement_data.move_force, 0.0f };
            }

            m_body.apply_force(m_movement_data.force_accumulator);

            m_world.update(dtf);

            m_movement_data.prev_velocity = m_movement_data.curr_velocity;
            m_movement_data.curr_velocity = m_body.velocity();

            m_movement_data.prev_acceleration = m_movement_data.curr_acceleration;
            m_movement_data.curr_acceleration = (m_movement_data.curr_velocity - m_movement_data.prev_velocity) / dtf;

            m_movement_data.force_accumulator = {};

            auto delta_acc = glm::abs(m_movement_data.curr_acceleration - m_movement_data.prev_acceleration);
            float eps = 0.0001f;
            if (delta_acc.x > eps || delta_acc.y > eps)
            {
                m_movement_records.push_back(movement_record{ .position = m_body.position(),
                                                              .velocity = m_body.velocity(),
                                                              .acceleration = m_movement_data.curr_acceleration,
                                                              .duration = 0.0f });
            }

            if (m_movement_records.size() > 0)
            {
                auto& rec = m_movement_records.back();
                rec.duration += 0.5f * dtf;
                m_movement_data.ghost_position = rec.position
                        + rec.velocity * rec.duration
                        + rec.acceleration * (0.5f * rec.duration * rec.duration);
                rec.duration += 0.5f * dtf;
            }
        }
        else
        {
            if (m_movement_records.size() > 0)
            {
                auto& rec = m_movement_records.back();
                rec.duration -= 0.5f * dtf;
                m_movement_data.ghost_position = rec.position
                        + rec.velocity * rec.duration
                        + rec.acceleration * (0.5f * rec.duration * rec.duration);
                rec.duration -= 0.5f * dtf;

                m_body.set_position(m_movement_data.ghost_position);
                m_body.set_position(m_movement_data.ghost_position);

                if (rec.duration <= 0.0f)
                {
                    m_movement_records.pop_back();
                }
            }
        }
    }

    void update(flow::duration /*dt*/) override
    {
        flow::gl::clear();

        m_renderer.begin_batch(m_camera.projection() * m_camera.view());

        for (const auto& body : m_static_bodies)
        {
            m_renderer.submit(body.position() * m_to_px,
                              body.extents() * (2.0f * m_to_px),
                              { 0.5f, 0.5f },
                              { 0.2f, 0.8f, 0.4f, 1.0f });
        }

        m_renderer.submit(m_body.position() * m_to_px,
                          m_body.extents() * (2.0f * m_to_px),
                          { 0.5f, 0.5f },
                          m_reversed_time ? glm::vec4{ 0.8f, 0.4f, 0.2f, 1.0f }
                                          : glm::vec4{ 0.2f, 0.4f, 0.8f, 1.0f });

        m_renderer.submit(initial_position * m_to_px,
                          m_body.extents() * (1.5f * m_to_px),
                          { 0.5f, 0.5f },
                          glm::vec4{ 0.2f, 0.8f, 0.4f, 1.0f });

        m_renderer.submit(m_movement_data.ghost_position * m_to_px,
                          m_body.extents() * m_to_px,
                          { 0.5f, 0.5f },
                          m_reversed_time ? glm::vec4{ 0.2f, 0.4f, 0.8f, 1.0f }
                                          : glm::vec4{ 0.8f, 0.4f, 0.2f, 1.0f });

        m_renderer.end_batch();
    }

private:
    flow::rectangle_renderer m_renderer{};
    flow::line_renderer m_line_renderer{};
    flow::orthographic_camera m_camera{};

    flow::world<glm::vec2> m_world{};
    flow::dynamic_body<glm::vec2> m_body{};
    std::vector<flow::static_body<glm::vec2>> m_static_bodies{};

    movement_data m_movement_data{};
    bool m_reversed_time{};
    std::vector<movement_record> m_movement_records{};
};
