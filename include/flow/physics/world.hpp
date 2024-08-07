#pragma once

#include <cstdint>
#include <memory>

#include <box2d/b2_contact.h>
#include <box2d/b2_world.h>

#include "body.hpp"

namespace flow {

template<typename Vec2T>
struct contact
{
    using vec2_type = Vec2T;

    body* body_a_ptr;
    body* body_b_ptr;
    vec2_type normal;
};

namespace detail {
    template<typename BeginContactCallbackT, typename EndContactCallbackT>
    class contact_listener : public b2ContactListener
    {
    public:
        using begin_contact_callback = BeginContactCallbackT;
        using end_contact_callback = EndContactCallbackT;

    public:
        contact_listener(begin_contact_callback begin_contact, end_contact_callback end_contact)
            : m_begin_contact_callback{ begin_contact }
            , m_end_contact_callback{ end_contact }
        {}

        void BeginContact(b2Contact* contact_ptr) override
        {
            m_begin_contact_callback({
                    .body_a_ptr = contact_ptr->GetFixtureA()->GetBody()->GetUserData().body_ptr,
                    .body_b_ptr = contact_ptr->GetFixtureB()->GetBody()->GetUserData().body_ptr,
                    .normal = { contact_ptr->GetManifold()->localNormal.x, contact_ptr->GetManifold()->localNormal.y },
            });
        }

        void EndContact(b2Contact* contact_ptr) override
        {
            m_end_contact_callback({
                    .body_a_ptr = contact_ptr->GetFixtureA()->GetBody()->GetUserData().body_ptr,
                    .body_b_ptr = contact_ptr->GetFixtureB()->GetBody()->GetUserData().body_ptr,
                    .normal = { contact_ptr->GetManifold()->localNormal.x, contact_ptr->GetManifold()->localNormal.y },
            });
        }

    private:
        begin_contact_callback m_begin_contact_callback;
        end_contact_callback m_end_contact_callback;
    };
} // namespace detail

template<typename Vec2T>
class world
{
public:
    using vec2_type = Vec2T;
    using value_type = typename Vec2T::value_type;

    static constexpr std::uint32_t default_velocity_iterations = 6;
    static constexpr std::uint32_t default_position_iterations = 2;

public:
    constexpr world() noexcept = default;

    world(const vec2_type& gravity, std::uint32_t velocity_iterations, std::uint32_t position_iterations) noexcept
    {
        create(gravity, velocity_iterations, position_iterations);
    }

    world(const vec2_type& gravity) noexcept
        : world(gravity, default_velocity_iterations, default_position_iterations)
    {}

    void create(const vec2_type& gravity, std::uint32_t velocity_iterations, std::uint32_t position_iterations) noexcept
    {
        m_world_ptr = std::make_unique<b2World>(b2Vec2{ gravity.x, gravity.y });
        m_velocity_iterations = velocity_iterations;
        m_position_iterations = position_iterations;
    }

    void create(const vec2_type& gravity) noexcept
    {
        create(gravity, default_velocity_iterations, default_position_iterations);
    }

    void update(value_type dt) noexcept
    {
        m_world_ptr->Step(static_cast<float>(dt),
                          static_cast<std::int32_t>(m_velocity_iterations),
                          static_cast<std::int32_t>(m_position_iterations));
    }

    template<typename BeginContactCallbackT, typename EndContactCallbackT>
    void set_contact_listener(BeginContactCallbackT begin_contact, EndContactCallbackT end_contact)
    {
        using listener_type = detail::contact_listener<BeginContactCallbackT, EndContactCallbackT>;

        m_contact_listener_ptr = std::make_unique<listener_type>(begin_contact, end_contact);
        m_world_ptr->SetContactListener(m_contact_listener_ptr.get());
    }

    constexpr void set_velocity_iterations(std::uint32_t iterations) noexcept
    {
        m_velocity_iterations = iterations;
    }

    constexpr void set_position_iterations(std::uint32_t iterations) noexcept
    {
        m_position_iterations = iterations;
    }

    [[nodiscard]] constexpr std::uint32_t velocity_iterations() const noexcept
    {
        return m_velocity_iterations;
    }

    [[nodiscard]] constexpr std::uint32_t position_iterations() const noexcept
    {
        return m_position_iterations;
    }

    [[nodiscard]] b2World* raw() const noexcept
    {
        return m_world_ptr.get();
    }

private:
    std::unique_ptr<b2World> m_world_ptr{};
    std::unique_ptr<b2ContactListener> m_contact_listener_ptr{};

    std::uint32_t m_velocity_iterations{ default_velocity_iterations };
    std::uint32_t m_position_iterations{ default_position_iterations };
};

} // namespace flow
