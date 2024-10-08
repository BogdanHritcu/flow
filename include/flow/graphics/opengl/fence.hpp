#pragma once

#include <glad/gl.h>

#include "../../utility/unique_handle.hpp"

namespace flow::gl {
class fence
{
public:
    using id_type = GLsync;
    using deleter_type = decltype([](id_type id)
    {
        glDeleteSync(id);
    });
    using handle_type = unique_handle<id_type, deleter_type>;

public:
    constexpr fence() noexcept = default;

    bool lock() noexcept
    {
        id_type id = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        m_handle.reset(id);

        return id != nullptr;
    }

    void wait() const noexcept
    {
        if (!m_handle.get())
        {
            return;
        }

        GLenum result{};

        do
        {
            result = glClientWaitSync(m_handle.get(), GL_SYNC_FLUSH_COMMANDS_BIT, 1);
        }
        while (result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED);
    }

private:
    handle_type m_handle{};
};
} // namespace flow::gl
