#pragma once

#include "input_binding.hpp"
#include "input_enums.hpp"
#include "input_system.hpp"
#include <string_view>

namespace flow {

class input_system_interface
{
public:
    using index_type = input_system::index_type;
    using callback_type = input_system::callback_type;

    // clang-format off

    static constexpr struct { using enum key_code;            } key;
    static constexpr struct { using enum mbtn_code;           } mbtn;
    static constexpr struct { using enum input_action_code;   } action;
    static constexpr struct { using enum input_modifier_code; } mod;

    // clang-format on

    static constexpr auto any = input_code::any;

public:
    constexpr input_system_interface()
        : m_input_system{}
    {}

    constexpr input_system_interface(input_system& input_system)
        : m_input_system{ &input_system }
    {}

    bool register_callback(std::string_view name,
                           const callback_type& callback)
    {
        return m_input_system->register_callback(name, callback);
    }

    void unregister_callback(std::string_view name)
    {
        m_input_system->unregister_callback(name);
    }

    bool register_context(std::string_view name)
    {
        return m_input_system->register_context(name);
    }

    void unregister_context(std::string_view name)
    {
        m_input_system->unregister_context(name);
    }

    bool register_binding(input_binding bind,
                          std::string_view callback_name,
                          std::string_view context_name)
    {
        return m_input_system->register_binding(bind, callback_name, context_name);
    }

    void unregister_binding(input_binding bind, std::string_view context_name)
    {
        m_input_system->unregister_binding(bind, context_name);
    }

    bool push_context(std::string_view name,
                      void* user_ptr = nullptr,
                      fallthrough_mode fallthrough = fallthrough_mode::never)
    {
        return m_input_system->push_context(name, user_ptr, fallthrough);
    }

    void pop_context()
    {
        m_input_system->pop_context();
    }

    [[nodiscard]] bool callback_exists(std::string_view name) const
    {
        return m_input_system->callback_exists(name);
    }

    [[nodiscard]] bool context_exists(std::string_view name) const
    {
        return m_input_system->context_exists(name);
    }

    [[nodiscard]] bool binding_exists(input_binding bind,
                                      std::string_view context_name) const
    {
        return m_input_system->binding_exists(bind, context_name);
    }

    [[nodiscard]] std::vector<input_binding> get_bindings(std::string_view callback_name,
                                                          std::string_view context_name)
    {
        return m_input_system->get_bindings(callback_name, context_name);
    }

private:
    input_system* m_input_system;
};

} // namespace flow
