#pragma once

#include <cstdint>
#include <utility>

#include <box2d/b2_body.h>

namespace flow {

class body
{
public:
    using tag_type = std::uint32_t;

    constexpr body() noexcept = default;

    body(const body& other) = delete;
    body& operator=(const body& other) = delete;

    body(body&& other) noexcept
        : m_body_ptr(std::exchange(other.m_body_ptr, nullptr))
        , m_tag(std::exchange(other.m_tag, 0))
    {
        m_body_ptr->GetUserData().body_ptr = this;
    }

    body& operator=(body&& other) noexcept
    {
        m_body_ptr = std::exchange(other.m_body_ptr, nullptr);
        m_tag = std::exchange(other.m_tag, 0);
        m_body_ptr->GetUserData().body_ptr = this;
    }

    virtual ~body() = default;

    [[nodiscard]] const tag_type& tag() const noexcept
    {
        return m_tag;
    }

protected:
    b2Body* m_body_ptr{};
    tag_type m_tag{};
};

} // namespace flow
