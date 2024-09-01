#pragma once

#include <cstddef>
#include <type_traits>

namespace flow {

template<typename T>
class invariant_ptr
{
public:
    using value_type = T;
    using pointer = value_type*;

public:
    constexpr invariant_ptr() noexcept = default;
    constexpr invariant_ptr(std::nullptr_t) noexcept
        : m_ptr{ nullptr }
    {}

    constexpr explicit invariant_ptr(pointer ptr) noexcept
        : m_ptr{ ptr }
    {}

    constexpr invariant_ptr(const invariant_ptr& other) noexcept {}
    constexpr invariant_ptr& operator=(const invariant_ptr& other) noexcept
    {
        return *this;
    }

    constexpr invariant_ptr(invariant_ptr&& other) noexcept {}
    constexpr invariant_ptr& operator=(invariant_ptr&& other) noexcept
    {
        return *this;
    }

    constexpr ~invariant_ptr() noexcept = default;

    constexpr void reset(pointer ptr = nullptr) noexcept
    {
        m_ptr = ptr;
    }

    [[nodiscard]] constexpr pointer get() const noexcept
    {
        return m_ptr;
    }

    [[nodiscard]] constexpr pointer get() noexcept
    {
        return m_ptr;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    [[nodiscard]] constexpr std::add_lvalue_reference_t<value_type> operator*() const
            noexcept(noexcept(*std::declval<pointer>()))
    {
        return *m_ptr;
    }

    [[nodiscard]] constexpr pointer operator->() const noexcept
    {
        return m_ptr;
    }

    [[nodiscard]] friend constexpr bool operator==(const invariant_ptr& a, const invariant_ptr& b) noexcept
    {
        return a.m_ptr == b.m_ptr;
    }

    [[nodiscard]] friend constexpr bool operator==(const invariant_ptr& a, pointer b) noexcept
    {
        return a.m_ptr == b;
    }

    [[nodiscard]] friend constexpr bool operator==(pointer a, const invariant_ptr& b) noexcept
    {
        return a == b.m_ptr;
    }

    [[nodiscard]] friend constexpr bool operator==(const invariant_ptr& a, std::nullptr_t) noexcept
    {
        return a.m_ptr == nullptr;
    }

    [[nodiscard]] friend constexpr bool operator==(std::nullptr_t, const invariant_ptr& b) noexcept
    {
        return nullptr == b.m_ptr;
    }

private:
    pointer m_ptr = nullptr;
};

} // namespace flow
