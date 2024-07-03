#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace flow {

class orthographic_camera
{
public:
    constexpr orthographic_camera() noexcept
        : m_view{ glm::identity<glm::mat4>() }
        , m_projection{ glm::identity<glm::mat4>() }
        , m_position{ 0.0f, 0.0f, 0.0f }
        , m_size{ 2.0f, 2.0f, 2.0f }
        , m_up{ 0.0f, 1.0f, 0.0f }
        , m_rotation{ 0.0f }
    {
    }

    orthographic_camera(float left, float right, float bottom, float top, float z_near = -1.0f, float z_far = 1.0f) noexcept
        : m_view{ glm::identity<glm::mat4>() }
        , m_projection{ glm::ortho(left, right, bottom, top, z_near, z_far) }
        , m_position{ 0.0f, 0.0f, 0.0f }
        , m_size{ glm::abs(right - left), glm::abs(top - bottom), glm::abs(z_far - z_near) }
        , m_up{ 0.0f, 1.0f, 0.0f }
        , m_rotation{ 0.0f }
    {
        update_rotation();
        update_view();
    }

    void set_position(const glm::vec3& position) noexcept
    {
        m_position = position;
        update_view();
    }

    void set_position(float x, float y, float z) noexcept
    {
        m_position = glm::vec3(x, y, z);
        update_view();
    }

    void set_position(const glm::vec2& position) noexcept
    {
        m_position.x = position.x;
        m_position.y = position.y;
        update_view();
    }

    void set_position(float x, float y) noexcept
    {
        m_position.x = x;
        m_position.y = y;
        update_view();
    }

    void set_z(float z) noexcept
    {
        m_position.z = z;
        update_view();
    }

    void move(const glm::vec3& value) noexcept
    {
        set_position(m_position + value);
    }

    void move(const glm::vec2& value) noexcept
    {
        set_position(glm::vec2(m_position.x, m_position.y) + value);
    }

    void set_rotation(float angle_deg) noexcept
    {
        m_rotation = angle_deg;
        update_rotation();
        update_view();
    }

    void rotate(float angle_deg) noexcept
    {
        set_rotation(m_rotation + angle_deg);
    }

    void set_projection(float left, float right, float bottom, float top, float z_near = -1.0f, float z_far = 1.0f) noexcept
    {
        m_projection = glm::ortho(left, right, bottom, top, z_near, z_far);
        m_size = glm::vec3(glm::abs(right - left), glm::abs(top - bottom), glm::abs(z_far - z_near));
    }

    [[nodiscard]] constexpr const glm::vec3& position() const noexcept
    {
        return m_position;
    }

    [[nodiscard]] constexpr float rotation() const noexcept
    {
        return m_rotation;
    }

    [[nodiscard]] constexpr const glm::mat4& projection() const noexcept
    {
        return m_projection;
    }

    [[nodiscard]] constexpr const glm::mat4& view() const noexcept
    {
        return m_view;
    }

private:
    void update_view() noexcept
    {
        // m_view = glm::lookAt(m_position, glm::vec3(m_position.x, m_position.y, m_position.z - 1.0f), m_up);

        glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), m_position);
        glm::mat4 rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));

        m_view = glm::inverse(translation * rotation);
    }

    void update_rotation()
    {
        // glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), -m_position);
        // glm::mat4 rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));

        // m_up = rotation * translation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

private:
    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::vec3 m_position;
    glm::vec3 m_size;
    glm::vec3 m_up;
    float m_rotation;
};

} // namespace flow
