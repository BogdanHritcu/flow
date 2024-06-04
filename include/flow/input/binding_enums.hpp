#pragma once

#include <cstdint>

#include "../utility/concepts.hpp"

namespace flow {

namespace detail {
    using binding_code_underlying_type = std::uint16_t;
    using binding_action_code_underlying_type = std::uint8_t;
    using binding_modifier_code_underlying_type = std::uint8_t;
} // namespace detail

enum class binding_code : detail::binding_code_underlying_type;
enum class key_code : detail::binding_code_underlying_type;
enum class mouse_code : detail::binding_code_underlying_type;
enum class binding_action_code : detail::binding_action_code_underlying_type;
enum class binding_modifier_code : detail::binding_modifier_code_underlying_type;

namespace concepts {
    template<typename T>
    concept binding_code = concepts::any_of<T, binding_code, key_code, mouse_code>;
}

namespace detail {
    inline constexpr binding_code_underlying_type binding_code_any_flag = binding_code_underlying_type{ 1 } << 15;

    template<concepts::binding_code T>
    inline constexpr binding_code_underlying_type binding_code_flag = 0;

    template<>
    inline constexpr binding_code_underlying_type binding_code_flag<key_code> = binding_code_underlying_type{ 1 } << 14;

    template<>
    inline constexpr binding_code_underlying_type binding_code_flag<mouse_code> = binding_code_underlying_type{ 1 } << 13;

    template<>
    inline constexpr binding_code_underlying_type binding_code_flag<binding_code> = binding_code_flag<key_code>
                                                                                  | binding_code_flag<mouse_code>;

} // namespace detail

// clang-format off

enum class binding_code : detail::binding_code_underlying_type
{
    any = detail::binding_code_flag<key_code>
        | detail::binding_code_flag<mouse_code>
        | detail::binding_code_any_flag
};

enum class key_code : detail::binding_code_underlying_type
{
    space         = detail::binding_code_flag<key_code> | 32,
    apostrophe    = detail::binding_code_flag<key_code> | 39,
    comma         = detail::binding_code_flag<key_code> | 44,
    minus         = detail::binding_code_flag<key_code> | 45,
    period        = detail::binding_code_flag<key_code> | 46,
    slash         = detail::binding_code_flag<key_code> | 47,
    key0          = detail::binding_code_flag<key_code> | 48,
    key1          = detail::binding_code_flag<key_code> | 49,
    key2          = detail::binding_code_flag<key_code> | 50,
    key3          = detail::binding_code_flag<key_code> | 51,
    key4          = detail::binding_code_flag<key_code> | 52,
    key5          = detail::binding_code_flag<key_code> | 53,
    key6          = detail::binding_code_flag<key_code> | 54,
    key7          = detail::binding_code_flag<key_code> | 55,
    key8          = detail::binding_code_flag<key_code> | 56,
    key9          = detail::binding_code_flag<key_code> | 57,
    semicolon     = detail::binding_code_flag<key_code> | 59,
    equal         = detail::binding_code_flag<key_code> | 61,
    a             = detail::binding_code_flag<key_code> | 65,
    b             = detail::binding_code_flag<key_code> | 66,
    c             = detail::binding_code_flag<key_code> | 67,
    d             = detail::binding_code_flag<key_code> | 68,
    e             = detail::binding_code_flag<key_code> | 69,
    f             = detail::binding_code_flag<key_code> | 70,
    g             = detail::binding_code_flag<key_code> | 71,
    h             = detail::binding_code_flag<key_code> | 72,
    i             = detail::binding_code_flag<key_code> | 73,
    j             = detail::binding_code_flag<key_code> | 74,
    k             = detail::binding_code_flag<key_code> | 75,
    l             = detail::binding_code_flag<key_code> | 76,
    m             = detail::binding_code_flag<key_code> | 77,
    n             = detail::binding_code_flag<key_code> | 78,
    o             = detail::binding_code_flag<key_code> | 79,
    p             = detail::binding_code_flag<key_code> | 80,
    q             = detail::binding_code_flag<key_code> | 81,
    r             = detail::binding_code_flag<key_code> | 82,
    s             = detail::binding_code_flag<key_code> | 83,
    t             = detail::binding_code_flag<key_code> | 84,
    u             = detail::binding_code_flag<key_code> | 85,
    v             = detail::binding_code_flag<key_code> | 86,
    w             = detail::binding_code_flag<key_code> | 87,
    x             = detail::binding_code_flag<key_code> | 88,
    y             = detail::binding_code_flag<key_code> | 89,
    z             = detail::binding_code_flag<key_code> | 90,
    left_bracket  = detail::binding_code_flag<key_code> | 91,
    backslash     = detail::binding_code_flag<key_code> | 92,
    right_bracket = detail::binding_code_flag<key_code> | 93,
    grave_accent  = detail::binding_code_flag<key_code> | 96,
    world1        = detail::binding_code_flag<key_code> | 161,
    world2        = detail::binding_code_flag<key_code> | 162,
    escape        = detail::binding_code_flag<key_code> | 256,
    enter         = detail::binding_code_flag<key_code> | 257,
    tab           = detail::binding_code_flag<key_code> | 258,
    backspace     = detail::binding_code_flag<key_code> | 259,
    insert        = detail::binding_code_flag<key_code> | 260,
    del           = detail::binding_code_flag<key_code> | 261,
    right         = detail::binding_code_flag<key_code> | 262,
    left          = detail::binding_code_flag<key_code> | 263,
    down          = detail::binding_code_flag<key_code> | 264,
    up            = detail::binding_code_flag<key_code> | 265,
    page_up       = detail::binding_code_flag<key_code> | 266,
    page_down     = detail::binding_code_flag<key_code> | 267,
    home          = detail::binding_code_flag<key_code> | 268,
    end           = detail::binding_code_flag<key_code> | 269,
    caps_lock     = detail::binding_code_flag<key_code> | 280,
    scroll_lock   = detail::binding_code_flag<key_code> | 281,
    num_lock      = detail::binding_code_flag<key_code> | 282,
    print_screen  = detail::binding_code_flag<key_code> | 283,
    pause         = detail::binding_code_flag<key_code> | 284,
    f1            = detail::binding_code_flag<key_code> | 290,
    f2            = detail::binding_code_flag<key_code> | 291,
    f3            = detail::binding_code_flag<key_code> | 292,
    f4            = detail::binding_code_flag<key_code> | 293,
    f5            = detail::binding_code_flag<key_code> | 294,
    f6            = detail::binding_code_flag<key_code> | 295,
    f7            = detail::binding_code_flag<key_code> | 296,
    f8            = detail::binding_code_flag<key_code> | 297,
    f9            = detail::binding_code_flag<key_code> | 298,
    f10           = detail::binding_code_flag<key_code> | 299,
    f11           = detail::binding_code_flag<key_code> | 300,
    f12           = detail::binding_code_flag<key_code> | 301,
    f13           = detail::binding_code_flag<key_code> | 302,
    f14           = detail::binding_code_flag<key_code> | 303,
    f15           = detail::binding_code_flag<key_code> | 304,
    f16           = detail::binding_code_flag<key_code> | 305,
    f17           = detail::binding_code_flag<key_code> | 306,
    f18           = detail::binding_code_flag<key_code> | 307,
    f19           = detail::binding_code_flag<key_code> | 308,
    f20           = detail::binding_code_flag<key_code> | 309,
    f21           = detail::binding_code_flag<key_code> | 310,
    f22           = detail::binding_code_flag<key_code> | 311,
    f23           = detail::binding_code_flag<key_code> | 312,
    f24           = detail::binding_code_flag<key_code> | 313,
    f25           = detail::binding_code_flag<key_code> | 314,
    num0          = detail::binding_code_flag<key_code> | 320,
    num1          = detail::binding_code_flag<key_code> | 321,
    num2          = detail::binding_code_flag<key_code> | 322,
    num3          = detail::binding_code_flag<key_code> | 323,
    num4          = detail::binding_code_flag<key_code> | 324,
    num5          = detail::binding_code_flag<key_code> | 325,
    num6          = detail::binding_code_flag<key_code> | 326,
    num7          = detail::binding_code_flag<key_code> | 327,
    num8          = detail::binding_code_flag<key_code> | 328,
    num9          = detail::binding_code_flag<key_code> | 329,
    num_decimal   = detail::binding_code_flag<key_code> | 330,
    num_divide    = detail::binding_code_flag<key_code> | 331,
    num_multiply  = detail::binding_code_flag<key_code> | 332,
    num_subtract  = detail::binding_code_flag<key_code> | 333,
    num_add       = detail::binding_code_flag<key_code> | 334,
    num_enter     = detail::binding_code_flag<key_code> | 335,
    num_equal     = detail::binding_code_flag<key_code> | 336,
    left_shift    = detail::binding_code_flag<key_code> | 340,
    left_control  = detail::binding_code_flag<key_code> | 341,
    left_alt      = detail::binding_code_flag<key_code> | 342,
    left_super    = detail::binding_code_flag<key_code> | 343,
    right_shift   = detail::binding_code_flag<key_code> | 344,
    right_control = detail::binding_code_flag<key_code> | 345,
    right_alt     = detail::binding_code_flag<key_code> | 346,
    right_super   = detail::binding_code_flag<key_code> | 347,
    menu          = detail::binding_code_flag<key_code> | 348,

    any           = detail::binding_code_flag<key_code>
                  | detail::binding_code_any_flag,
};

enum class mouse_code : detail::binding_code_underlying_type
{
    b1       = detail::binding_code_flag<mouse_code> | 0,
    b2       = detail::binding_code_flag<mouse_code> | 1,
    b3       = detail::binding_code_flag<mouse_code> | 2,
    b4       = detail::binding_code_flag<mouse_code> | 3,
    b5       = detail::binding_code_flag<mouse_code> | 4,
    b6       = detail::binding_code_flag<mouse_code> | 5,
    b7       = detail::binding_code_flag<mouse_code> | 6,
    b8       = detail::binding_code_flag<mouse_code> | 7,
    left     = b1,
    right    = b2,
    middle   = b3,

    any      = detail::binding_code_flag<mouse_code>
             | detail::binding_code_any_flag,
};

enum class binding_action_code : detail::binding_action_code_underlying_type
{
    release = 0,
    press   = 1,
    repeat  = 2,
};

enum class binding_modifier_code : detail::binding_modifier_code_underlying_type
{
    none      = 0x0000,
    shift     = 0x0001,
    control   = 0x0002,
    alt       = 0x0004,
    super     = 0x0008,
    caps_lock = 0x0010,
    num_lock  = 0x0020,
};

// clang-format on

[[nodiscard]] constexpr binding_modifier_code operator|(binding_modifier_code lhs,
                                                        binding_modifier_code rhs) noexcept
{
    return static_cast<binding_modifier_code>(static_cast<detail::binding_modifier_code_underlying_type>(lhs)
                                              | static_cast<detail::binding_modifier_code_underlying_type>(rhs));
}

namespace detail {
    template<concepts::binding_code T>
    [[nodiscard]] constexpr bool is_code_type(binding_code code) noexcept
    {
        return static_cast<binding_code_underlying_type>(code) & binding_code_flag<T>;
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr bool is_code_any(binding_code code) noexcept
    {
        return static_cast<binding_code_underlying_type>(code)
             & binding_code_flag<T>
             & binding_code_any_flag;
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr T add_code_flag(binding_code_underlying_type code) noexcept
    {
        return static_cast<T>(code | binding_code_flag<T>);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr T add_any_flag(T code) noexcept
    {
        return static_cast<T>(static_cast<binding_code_underlying_type>(code)
                              | binding_code_any_flag);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr T remove_any_flag(T code) noexcept
    {
        return static_cast<T>(static_cast<binding_code_underlying_type>(code)
                              | ~binding_code_any_flag);
    }

    template<concepts::binding_code T>
    [[nodiscard]] constexpr T to_any_code(T code) noexcept
    {
        return static_cast<T>((static_cast<binding_code_underlying_type>(code) & binding_code_flag<T>)
                              | binding_code_any_flag);
    }

} // namespace detail

} // namespace flow
