#pragma once

#include <string_view>

#include <flow/core/application.hpp>
#include <flow/core/engine_interface.hpp>
#include <flow/core/logger.hpp>
#include <flow/input/binding.hpp>
#include <flow/input/binding_enums.hpp>
#include <flow/utility/integer_range.hpp>

constexpr std::string_view context_start_screen = "start_screen";
constexpr std::string_view context_main_menu = "main_menu";
constexpr std::string_view context_gameplay = "gameplay";

constexpr std::string_view action_start_game = "start_game";

constexpr std::string_view action_close_game = "close_game";
constexpr std::string_view action_open_main_menu = "open_main_menu";
constexpr std::string_view action_exit_main_menu = "exit_main_menu";

constexpr std::string_view action_jump = "jump";
constexpr std::string_view action_attack = "attack";
constexpr std::string_view action_special_attack = "special_attack";
constexpr std::string_view action_move_left = "move_left";
constexpr std::string_view action_move_right = "move_right";

class input_system_test final : public flow::application
{
public:
    void setup() final
    {
        engine.window.set_title("input_context_test");
        engine.window.set_size(640, 480); // NOLINT
    }

    void start() final
    {
        auto callback_start_game = [](flow::engine_interface engine) -> void
        {
            FLOW_LOG_INFO("started game"); // NOLINT

            // engine.input.pop_binding_context();
            engine.input.push_binding_context(context_gameplay);
        };

        auto callback_close_game = [](flow::engine_interface engine) -> void
        {
            FLOW_LOG_INFO("closed game"); // NOLINT

            engine.quit();
        };

        auto callback_open_main_menu = [](flow::engine_interface engine) -> void
        {
            FLOW_LOG_INFO("gameplay -> main menu"); // NOLINT

            engine.input.push_binding_context(context_main_menu);
        };

        auto callback_exit_main_menu = [](flow::engine_interface engine) -> void
        {
            FLOW_LOG_INFO("main menu -> gameplay"); // NOLINT

            engine.input.pop_binding_context();
            engine.input.push_binding_context(context_gameplay);
        };

        auto callback_jump = [&](flow::engine_interface /*engine*/) -> void
        {
            FLOW_LOG_INFO("jumped {}m", m_jump_height); // NOLINT
        };

        auto callback_attack = [&](flow::engine_interface /*engine*/) -> void
        {
            FLOW_LOG_INFO("attacked (normal: {}dmg)", m_attack_damage); // NOLINT
        };

        auto callback_special_attack = [&](flow::engine_interface /*engine*/) -> void
        {
            FLOW_LOG_INFO("attacked (special: {}dmg)", m_attack_damage * 3.0f); // NOLINT
        };

        auto callback_move_left = [](flow::engine_interface /*engine*/) -> void
        {
            FLOW_LOG_INFO("moved left"); // NOLINT
        };

        auto callback_move_right = [](flow::engine_interface /*engine*/) -> void
        {
            FLOW_LOG_INFO("moved right"); // NOLINT
        };

        // input contexts registration
        engine.input.register_binding_context(context_start_screen);
        engine.input.register_binding_context(context_main_menu);
        engine.input.register_binding_context(context_gameplay);

        // callbacks registration
        engine.input.register_binding_callback(action_start_game, callback_start_game);

        engine.input.register_binding_callback(action_close_game, callback_close_game);
        engine.input.register_binding_callback(action_open_main_menu, callback_open_main_menu);
        engine.input.register_binding_callback(action_exit_main_menu, callback_exit_main_menu);

        engine.input.register_binding_callback(action_attack, callback_attack);
        engine.input.register_binding_callback(action_special_attack, callback_special_attack);
        engine.input.register_binding_callback(action_jump, callback_jump);
        engine.input.register_binding_callback(action_move_left, callback_move_left);
        engine.input.register_binding_callback(action_move_right, callback_move_right);

        // bindings registration

        // start screen
        engine.input.register_binding(flow::binding{
                                          engine.input.any,
                                          engine.action.press,
                                          engine.mod.none,
                                      },
                                      action_start_game,
                                      context_start_screen);

        // main menu
        engine.input.register_binding(flow::binding{
                                          engine.key.escape,
                                          engine.action.press,
                                          engine.mod.shift,
                                      },
                                      action_close_game,
                                      context_main_menu);

        engine.input.register_binding(flow::binding{
                                          engine.key.space,
                                          engine.action.press,
                                          engine.mod.none,
                                      },
                                      action_exit_main_menu,
                                      context_main_menu);

        // gameplay
        engine.input.register_binding(flow::binding{
                                          engine.key.escape,
                                          engine.action.press,
                                          engine.mod.none,
                                      },
                                      action_open_main_menu,
                                      context_gameplay);

        engine.input.register_binding(flow::binding{
                                          engine.mouse.left,
                                          engine.action.press,
                                          engine.mod.none,
                                      },
                                      action_attack,
                                      context_gameplay);

        engine.input.register_binding(flow::binding{
                                          engine.mouse.left,
                                          engine.action.press,
                                          engine.mod.control | engine.mod.alt,
                                      },
                                      action_special_attack,
                                      context_gameplay);

        engine.input.register_binding(flow::binding{
                                          engine.key.space,
                                          engine.action.press,
                                          engine.mod.none,
                                      },
                                      action_jump,
                                      context_gameplay);

        engine.input.register_binding(flow::binding{
                                          engine.key.a,
                                          engine.action.press,
                                          engine.mod.none,
                                      },
                                      action_move_left,
                                      context_gameplay);

        engine.input.register_binding(flow::binding{
                                          engine.key.d,
                                          engine.action.press,
                                          engine.mod.none,
                                      },
                                      action_move_right,
                                      context_gameplay);

        engine.input.push_binding_context(context_start_screen);

        FLOW_LOG_INFO("start screen");
        FLOW_LOG_INFO("press any key or button to start");
    }

private:
    float m_attack_damage = 30.0f;
    float m_jump_height = 0.4f;
};
