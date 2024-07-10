#pragma once

#include <utility>

#include "compressed_pair.hpp"
#include "concepts.hpp"

namespace flow {

template<concepts::nothrow_default_constructible HandleT,
         concepts::nothrow_default_constructible DeleterT>
class unique_handle
{
public:
    using handle_type = HandleT;
    using deleter_type = DeleterT;

public:
    constexpr unique_handle(handle_type handle) noexcept
        : m_pair{ handle, deleter_type{} }
    {}

    constexpr unique_handle() noexcept
        : unique_handle(handle_type{})
    {}

    constexpr unique_handle(const unique_handle& other) = delete;
    constexpr unique_handle& operator=(const unique_handle& other) = delete;

    constexpr unique_handle(unique_handle&& other) noexcept
        : m_pair{ other.release(), other.get_deleter() }
    {}

    constexpr unique_handle& operator=(unique_handle&& other) noexcept
    {
        if (this != &other)
        {
            reset(other.release());
        }

        return *this;
    }

    constexpr ~unique_handle() noexcept(noexcept(m_pair.second(m_pair.first)))
    {
        if (m_pair.first)
        {
            m_pair.second(m_pair.first);
        }
    }

    constexpr void swap(unique_handle& other) noexcept
    {
        using std::swap;
        swap(m_pair, other.m_pair);
    }

    [[nodiscard]] constexpr handle_type get() const noexcept
    {
        return m_pair.first;
    }

    [[nodiscard]] constexpr deleter_type& get_deleter() noexcept
    {
        return m_pair.second;
    }

    [[nodiscard]] constexpr const deleter_type& get_deleter() const noexcept
    {
        return m_pair.second;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return static_cast<bool>(m_pair.first);
    }

    constexpr handle_type release() noexcept
    {
        return std::exchange(m_pair.first, handle_type{});
    }

    constexpr void reset(handle_type id = handle_type{}) noexcept
    {
        handle_type old = std::exchange(m_pair.first, id);

        if (old)
        {
            m_pair.second(old);
        }
    }

private:
    compressed_pair<handle_type, deleter_type> m_pair;
};

} // namespace flow
