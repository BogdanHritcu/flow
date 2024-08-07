#pragma once

#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

#include "body.hpp"
#include "world.hpp"

namespace flow {

template<typename Vec2T>
class static_body : public body
{
public:
    using vec2_type = Vec2T;
    using value_type = typename Vec2T::value_type;

public:
    constexpr static_body() noexcept = default;

    static_body(world<vec2_type>& world, const vec2_type& position, const vec2_type& extents, tag_type tag = {}) noexcept
    {
        create(world, position, extents, tag);
    }

    void create(world<vec2_type>& world, const vec2_type& position, const vec2_type& extents, tag_type tag = {}) noexcept
    {
        if (m_body_ptr)
        {
            world.raw()->DestroyBody(m_body_ptr);
        }

        b2BodyDef body_def{};
        body_def.type = b2BodyType::b2_staticBody;
        body_def.position = { position.x, position.y };
        body_def.fixedRotation = true;

        m_extents = extents;
        b2PolygonShape shape{};
        shape.SetAsBox(m_extents.x, m_extents.y);

        m_body_ptr = world.raw()->CreateBody(&body_def);
        m_body_ptr->CreateFixture(&shape, 0.0f);

        m_body_ptr->GetUserData().body_ptr = this;
        m_tag = tag;
    }

    void set_position(const vec2_type& position) noexcept
    {
        m_body_ptr->SetTransform({ position.x, position.y }, 0.0f);
    }

    [[nodiscard]] vec2_type position() const noexcept
    {
        return vec2_type{ m_body_ptr->GetPosition().x, m_body_ptr->GetPosition().y };
    }

    [[nodiscard]] const vec2_type& extents() const noexcept
    {
        return m_extents;
    }

private:
    vec2_type m_extents{};
};

} // namespace flow
