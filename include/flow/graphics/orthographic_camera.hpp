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
    constexpr orthographic_camera() noexcept = default;

    orthographic_camera(float left, float right, float bottom, float top, float z_near = -1.0f, float z_far = 1.0f) noexcept
        : m_projection{ glm::ortho(left, right, bottom, top, z_near, z_far) }
        , m_view{ glm::identity<glm::mat4>() }
        , m_position{ 0.0f, 0.0f, 0.0f }
        , m_up{ 0.0f, 1.0f, 0.0f }
    {
        update_view();
    }

    void set_position(float x, float y, float z) noexcept
    {
        m_position.x = x;
        m_position.y = y;
        m_position.z = z;
        update_view();
    }

    void set_position(float x, float y) noexcept
    {
        set_position(x, y, m_position.z);
    }

    void set_position(const glm::vec3& position) noexcept
    {
        set_position(position.x, position.y, position.z);
    }

    void set_position(const glm::vec2& position) noexcept
    {
        set_position(position.x, position.y);
    }

    void set_z(float z) noexcept
    {
        set_position(m_position.x, m_position.y, z);
    }

    void move(const glm::vec3& off) noexcept
    {
        set_position(m_position + off);
    }

    void move(float x_off, float y_off, float z_off) noexcept
    {
        move({ x_off, y_off, z_off });
    }

    void move(float x_off, float y_off) noexcept
    {
        move(x_off, y_off, 0.0f);
    }

    void move(const glm::vec2& off) noexcept
    {
        move(off.x, off.y);
    }

    void set_projection(float left, float right, float bottom, float top, float z_near = -1.0f, float z_far = 1.0f) noexcept
    {
        m_projection = glm::ortho(left, right, bottom, top, z_near, z_far);
        update_view_projection();
    }

    [[nodiscard]] constexpr const glm::vec3& position() const noexcept
    {
        return m_position;
    }

    [[nodiscard]] constexpr const glm::mat4& projection() const noexcept
    {
        return m_projection;
    }

    [[nodiscard]] constexpr const glm::mat4& view() const noexcept
    {
        return m_view;
    }

    [[nodiscard]] constexpr const glm::mat4& view_projection() const noexcept
    {
        return m_view_projection;
    }

    [[nodiscard]] constexpr const glm::mat4& inv_view_projection() const noexcept
    {
        return m_inv_view_projection;
    }

private:
    void update_view() noexcept
    {
        m_view = glm::lookAt(m_position, glm::vec3(m_position.x, m_position.y, m_position.z - 1.0f), m_up);
        update_view_projection();
    }

    void update_view_projection() noexcept
    {
        m_view_projection = m_projection * m_view;
        m_inv_view_projection = glm::inverse(m_view_projection);
    }

private:
    glm::mat4 m_projection{ glm::identity<glm::mat4>() };
    glm::mat4 m_view{ glm::identity<glm::mat4>() };
    glm::mat4 m_view_projection{ glm::identity<glm::mat4>() };
    glm::mat4 m_inv_view_projection{ glm::identity<glm::mat4>() };
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };
};

} // namespace flow
