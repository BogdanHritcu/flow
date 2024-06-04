#pragma once

#include <string_view>

#include "../input/binding.hpp"
#include "../input/binding_enums.hpp"
#include "input_system.hpp"

namespace flow {

class input_system_interface
{
public:
    using index_type = input_system::index_type;
    using binding_callback_type = input_system::binding_callback_type;
    using input_context_type = input_system::input_context;

    // clang-format off

    static constexpr struct { using enum key_code;            } key;
    static constexpr struct { using enum mouse_code;           } mouse;
    static constexpr struct { using enum binding_action_code;   } action;
    static constexpr struct { using enum binding_modifier_code; } mod;

    // clang-format on

    static constexpr auto any = binding_code::any;

public:
    constexpr input_system_interface()
        : m_input_system{}
    {}

    constexpr input_system_interface(input_system& input_system)
        : m_input_system{ &input_system }
    {}

    bool register_binding_callback(std::string_view name,
                                   const binding_callback_type& callback)
    {
        return m_input_system->register_binding_callback(name, callback);
    }

    void unregister_binding_callback(std::string_view name)
    {
        m_input_system->unregister_binding_callback(name);
    }

    bool register_binding_context(std::string_view name)
    {
        return m_input_system->register_binding_context(name);
    }

    void unregister_binding_context(std::string_view name)
    {
        m_input_system->unregister_binding_context(name);
    }

    bool register_binding(binding bind,
                          std::string_view callback_name,
                          std::string_view context_name)
    {
        return m_input_system->register_binding(bind, callback_name, context_name);
    }

    void unregister_binding(binding bind, std::string_view context_name)
    {
        m_input_system->unregister_binding(bind, context_name);
    }

    bool push_binding_context(std::string_view name,
                              void* user_ptr = nullptr,
                              fallthrough_mode fallthrough = fallthrough_mode::never)
    {
        return m_input_system->push_binding_context(name, user_ptr, fallthrough);
    }

    void pop_binding_context()
    {
        m_input_system->pop_binding_context();
    }

    [[nodiscard]] bool binding_callback_exists(std::string_view name) const
    {
        return m_input_system->binding_callback_exists(name);
    }

    [[nodiscard]] bool binding_context_exists(std::string_view name) const
    {
        return m_input_system->binding_context_exists(name);
    }

    [[nodiscard]] bool binding_exists(binding bind,
                                      std::string_view context_name) const
    {
        return m_input_system->binding_exists(bind, context_name);
    }

    [[nodiscard]] std::vector<binding> get_bindings(std::string_view callback_name,
                                                    std::string_view context_name)
    {
        return m_input_system->get_bindings(callback_name, context_name);
    }

    [[nodiscard]] constexpr const input_context_type& context() const noexcept
    {
        return m_input_system->context();
    }

private:
    input_system* m_input_system;
};

} // namespace flow
