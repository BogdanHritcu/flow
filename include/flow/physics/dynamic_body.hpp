#pragma once

#include <cstdint>

#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

#include "body.hpp"
#include "world.hpp"

namespace flow {

template<typename Vec2T>
class dynamic_body : public body
{
public:
    using vec2_type = Vec2T;
    using value_type = typename Vec2T::value_type;

public:
    constexpr dynamic_body() noexcept = default;

    dynamic_body(world<vec2_type>& world, const vec2_type& position, const vec2_type& extents, value_type density, value_type friction, tag_type tag = {}) noexcept
    {
        create(world, position, extents, density, friction, tag);
    }

    void create(world<vec2_type>& world, const vec2_type& position, const vec2_type& extents, value_type density, value_type friction, tag_type tag = {}) noexcept
    {
        if (m_body_ptr)
        {
            world.raw()->DestroyBody(m_body_ptr);
        }

        b2BodyDef body_def{};
        body_def.type = b2BodyType::b2_dynamicBody;
        body_def.position = { position.x, position.y };
        body_def.fixedRotation = true;

        m_extents = extents;
        b2PolygonShape shape{};
        shape.SetAsBox(m_extents.x, m_extents.y);

        b2FixtureDef fixture_def{};
        fixture_def.shape = &shape;
        fixture_def.density = density;
        fixture_def.friction = static_cast<float>(friction);

        m_body_ptr = world.raw()->CreateBody(&body_def);
        m_body_ptr->CreateFixture(&fixture_def);

        m_body_ptr->GetUserData().body_ptr = this;
        m_tag = tag;
    }

    void set_position(const vec2_type& position) noexcept
    {
        m_body_ptr->SetTransform({ position.x, position.y }, 0.0f);
    }

    void set_velocity(const vec2_type& velocity) noexcept
    {
        m_body_ptr->SetLinearVelocity({ velocity.x, velocity.y });
    }

    void apply_force(const vec2_type& force) noexcept
    {
        m_body_ptr->ApplyForceToCenter({ force.x, force.y }, true);
    }

    void apply_impulse(const vec2_type& impulse) noexcept
    {
        m_body_ptr->ApplyLinearImpulseToCenter({ impulse.x, impulse.y }, true);
    }

    [[nodiscard]] vec2_type position() const noexcept
    {
        return vec2_type{ m_body_ptr->GetPosition().x, m_body_ptr->GetPosition().y };
    }

    [[nodiscard]] vec2_type velocity() const noexcept
    {
        return vec2_type{ m_body_ptr->GetLinearVelocity().x, m_body_ptr->GetLinearVelocity().y };
    }

    [[nodiscard]] value_type mass() const noexcept
    {
        return m_body_ptr->GetMass();
    }

    [[nodiscard]] const vec2_type& extents() const noexcept
    {
        return m_extents;
    }

private:
    vec2_type m_extents{};
};

} // namespace flow
