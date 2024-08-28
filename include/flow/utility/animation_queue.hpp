#pragma once

#include <memory>
#include <queue>
#include <utility>

#include "animation.hpp"

namespace flow {

class animation_queue
{
public:
    void advance(flow::duration dt) noexcept(noexcept(m_animations.front()->advance(dt)))
    {
        if (m_animations.empty())
        {
            return;
        }

        m_animations.front()->advance(dt);

        if (m_animations.front()->has_finished())
        {
            m_animations.pop();
        }
    }

    void push(std::unique_ptr<animation> animation)
    {
        m_animations.push(std::move(animation));
    }

    template<typename AnimationT, typename... ArgsT>
    void emplace(ArgsT&&... args)
    {
        m_animations.emplace(std::make_unique<AnimationT>(std::forward<ArgsT>(args)...));
    }

    void pop() noexcept(noexcept(m_animations.pop()))
    {
        m_animations.pop();
    }

    [[nodiscard]] decltype(auto) size() const noexcept(noexcept(m_animations.size()))
    {
        return m_animations.size();
    }

    [[nodiscard]] bool empty() const noexcept(noexcept(m_animations.empty()))
    {
        return m_animations.empty();
    }

private:
    std::queue<std::unique_ptr<animation>> m_animations;
};

} // namespace flow
