#pragma once

#include <cstdint>

#include "../utility/concepts.hpp"

namespace flow {

namespace detail {
    using input_code_underlying_type = std::uint16_t;
    using input_action_code_underlying_type = std::uint8_t;
    using input_modifier_code_underlying_type = std::uint8_t;
} // namespace detail

enum class input_code : detail::input_code_underlying_type;
enum class key_code : detail::input_code_underlying_type;
enum class mbtn_code : detail::input_code_underlying_type;
enum class input_action_code : detail::input_action_code_underlying_type;
enum class input_modifier_code : detail::input_modifier_code_underlying_type;

namespace concepts {
    template<typename T>
    concept input_code = concepts::any_of<T, input_code, key_code, mbtn_code>;
}

namespace detail {
    inline constexpr input_code_underlying_type input_code_any_flag = input_code_underlying_type{ 1 } << 15;

    template<concepts::input_code T>
    inline constexpr input_code_underlying_type input_code_flag = 0;

    template<>
    inline constexpr input_code_underlying_type input_code_flag<key_code> = input_code_underlying_type{ 1 } << 14;

    template<>
    inline constexpr input_code_underlying_type input_code_flag<mbtn_code> = input_code_underlying_type{ 1 } << 13;

    template<>
    inline constexpr input_code_underlying_type input_code_flag<input_code> = input_code_flag<key_code>
                                                                            | input_code_flag<mbtn_code>;

} // namespace detail

// clang-format off

enum class input_code : detail::input_code_underlying_type
{
    any = detail::input_code_flag<key_code>
        | detail::input_code_flag<mbtn_code>
        | detail::input_code_any_flag
};

enum class key_code : detail::input_code_underlying_type
{
    space         = detail::input_code_flag<key_code> | 32,
    apostrophe    = detail::input_code_flag<key_code> | 39,
    comma         = detail::input_code_flag<key_code> | 44,
    minus         = detail::input_code_flag<key_code> | 45,
    period        = detail::input_code_flag<key_code> | 46,
    slash         = detail::input_code_flag<key_code> | 47,
    key0          = detail::input_code_flag<key_code> | 48,
    key1          = detail::input_code_flag<key_code> | 49,
    key2          = detail::input_code_flag<key_code> | 50,
    key3          = detail::input_code_flag<key_code> | 51,
    key4          = detail::input_code_flag<key_code> | 52,
    key5          = detail::input_code_flag<key_code> | 53,
    key6          = detail::input_code_flag<key_code> | 54,
    key7          = detail::input_code_flag<key_code> | 55,
    key8          = detail::input_code_flag<key_code> | 56,
    key9          = detail::input_code_flag<key_code> | 57,
    semicolon     = detail::input_code_flag<key_code> | 59,
    equal         = detail::input_code_flag<key_code> | 61,
    a             = detail::input_code_flag<key_code> | 65,
    b             = detail::input_code_flag<key_code> | 66,
    c             = detail::input_code_flag<key_code> | 67,
    d             = detail::input_code_flag<key_code> | 68,
    e             = detail::input_code_flag<key_code> | 69,
    f             = detail::input_code_flag<key_code> | 70,
    g             = detail::input_code_flag<key_code> | 71,
    h             = detail::input_code_flag<key_code> | 72,
    i             = detail::input_code_flag<key_code> | 73,
    j             = detail::input_code_flag<key_code> | 74,
    k             = detail::input_code_flag<key_code> | 75,
    l             = detail::input_code_flag<key_code> | 76,
    m             = detail::input_code_flag<key_code> | 77,
    n             = detail::input_code_flag<key_code> | 78,
    o             = detail::input_code_flag<key_code> | 79,
    p             = detail::input_code_flag<key_code> | 80,
    q             = detail::input_code_flag<key_code> | 81,
    r             = detail::input_code_flag<key_code> | 82,
    s             = detail::input_code_flag<key_code> | 83,
    t             = detail::input_code_flag<key_code> | 84,
    u             = detail::input_code_flag<key_code> | 85,
    v             = detail::input_code_flag<key_code> | 86,
    w             = detail::input_code_flag<key_code> | 87,
    x             = detail::input_code_flag<key_code> | 88,
    y             = detail::input_code_flag<key_code> | 89,
    z             = detail::input_code_flag<key_code> | 90,
    left_bracket  = detail::input_code_flag<key_code> | 91,
    backslash     = detail::input_code_flag<key_code> | 92,
    right_bracket = detail::input_code_flag<key_code> | 93,
    grave_accent  = detail::input_code_flag<key_code> | 96,
    world1        = detail::input_code_flag<key_code> | 161,
    world2        = detail::input_code_flag<key_code> | 162,
    escape        = detail::input_code_flag<key_code> | 256,
    enter         = detail::input_code_flag<key_code> | 257,
    tab           = detail::input_code_flag<key_code> | 258,
    backspace     = detail::input_code_flag<key_code> | 259,
    insert        = detail::input_code_flag<key_code> | 260,
    del           = detail::input_code_flag<key_code> | 261,
    right         = detail::input_code_flag<key_code> | 262,
    left          = detail::input_code_flag<key_code> | 263,
    down          = detail::input_code_flag<key_code> | 264,
    up            = detail::input_code_flag<key_code> | 265,
    page_up       = detail::input_code_flag<key_code> | 266,
    page_down     = detail::input_code_flag<key_code> | 267,
    home          = detail::input_code_flag<key_code> | 268,
    end           = detail::input_code_flag<key_code> | 269,
    caps_lock     = detail::input_code_flag<key_code> | 280,
    scroll_lock   = detail::input_code_flag<key_code> | 281,
    num_lock      = detail::input_code_flag<key_code> | 282,
    print_screen  = detail::input_code_flag<key_code> | 283,
    pause         = detail::input_code_flag<key_code> | 284,
    f1            = detail::input_code_flag<key_code> | 290,
    f2            = detail::input_code_flag<key_code> | 291,
    f3            = detail::input_code_flag<key_code> | 292,
    f4            = detail::input_code_flag<key_code> | 293,
    f5            = detail::input_code_flag<key_code> | 294,
    f6            = detail::input_code_flag<key_code> | 295,
    f7            = detail::input_code_flag<key_code> | 296,
    f8            = detail::input_code_flag<key_code> | 297,
    f9            = detail::input_code_flag<key_code> | 298,
    f10           = detail::input_code_flag<key_code> | 299,
    f11           = detail::input_code_flag<key_code> | 300,
    f12           = detail::input_code_flag<key_code> | 301,
    f13           = detail::input_code_flag<key_code> | 302,
    f14           = detail::input_code_flag<key_code> | 303,
    f15           = detail::input_code_flag<key_code> | 304,
    f16           = detail::input_code_flag<key_code> | 305,
    f17           = detail::input_code_flag<key_code> | 306,
    f18           = detail::input_code_flag<key_code> | 307,
    f19           = detail::input_code_flag<key_code> | 308,
    f20           = detail::input_code_flag<key_code> | 309,
    f21           = detail::input_code_flag<key_code> | 310,
    f22           = detail::input_code_flag<key_code> | 311,
    f23           = detail::input_code_flag<key_code> | 312,
    f24           = detail::input_code_flag<key_code> | 313,
    f25           = detail::input_code_flag<key_code> | 314,
    num0          = detail::input_code_flag<key_code> | 320,
    num1          = detail::input_code_flag<key_code> | 321,
    num2          = detail::input_code_flag<key_code> | 322,
    num3          = detail::input_code_flag<key_code> | 323,
    num4          = detail::input_code_flag<key_code> | 324,
    num5          = detail::input_code_flag<key_code> | 325,
    num6          = detail::input_code_flag<key_code> | 326,
    num7          = detail::input_code_flag<key_code> | 327,
    num8          = detail::input_code_flag<key_code> | 328,
    num9          = detail::input_code_flag<key_code> | 329,
    num_decimal   = detail::input_code_flag<key_code> | 330,
    num_divide    = detail::input_code_flag<key_code> | 331,
    num_multiply  = detail::input_code_flag<key_code> | 332,
    num_subtract  = detail::input_code_flag<key_code> | 333,
    num_add       = detail::input_code_flag<key_code> | 334,
    num_enter     = detail::input_code_flag<key_code> | 335,
    num_equal     = detail::input_code_flag<key_code> | 336,
    left_shift    = detail::input_code_flag<key_code> | 340,
    left_control  = detail::input_code_flag<key_code> | 341,
    left_alt      = detail::input_code_flag<key_code> | 342,
    left_super    = detail::input_code_flag<key_code> | 343,
    right_shift   = detail::input_code_flag<key_code> | 344,
    right_control = detail::input_code_flag<key_code> | 345,
    right_alt     = detail::input_code_flag<key_code> | 346,
    right_super   = detail::input_code_flag<key_code> | 347,
    menu          = detail::input_code_flag<key_code> | 348,

    any           = detail::input_code_flag<key_code>
                  | detail::input_code_any_flag,
};

enum class mbtn_code : detail::input_code_underlying_type
{
    b1       = detail::input_code_flag<mbtn_code> | 0,
    b2       = detail::input_code_flag<mbtn_code> | 1,
    b3       = detail::input_code_flag<mbtn_code> | 2,
    b4       = detail::input_code_flag<mbtn_code> | 3,
    b5       = detail::input_code_flag<mbtn_code> | 4,
    b6       = detail::input_code_flag<mbtn_code> | 5,
    b7       = detail::input_code_flag<mbtn_code> | 6,
    b8       = detail::input_code_flag<mbtn_code> | 7,
    left     = b1,
    right    = b2,
    middle   = b3,

    any      = detail::input_code_flag<mbtn_code>
             | detail::input_code_any_flag,
};

enum class input_action_code : detail::input_action_code_underlying_type
{
    release = 0,
    press   = 1,
    repeat  = 2,
};

enum class input_modifier_code : detail::input_modifier_code_underlying_type
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

[[nodiscard]] constexpr input_modifier_code operator|(input_modifier_code lhs,
                                                      input_modifier_code rhs) noexcept
{
    return static_cast<input_modifier_code>(static_cast<detail::input_modifier_code_underlying_type>(lhs)
                                            | static_cast<detail::input_modifier_code_underlying_type>(rhs));
}

namespace detail {
    template<concepts::input_code T>
    [[nodiscard]] constexpr bool is_code_type(input_code code) noexcept
    {
        return static_cast<input_code_underlying_type>(code) & input_code_flag<T>;
    }

    template<concepts::input_code T>
    [[nodiscard]] constexpr bool is_code_any(input_code code) noexcept
    {
        return static_cast<input_code_underlying_type>(code)
             & input_code_flag<T>
             & input_code_any_flag;
    }

    template<concepts::input_code T>
    [[nodiscard]] constexpr T add_code_flag(input_code_underlying_type code) noexcept
    {
        return static_cast<T>(code | input_code_flag<T>);
    }

    template<concepts::input_code T>
    [[nodiscard]] constexpr T add_any_flag(T code) noexcept
    {
        return static_cast<T>(static_cast<input_code_underlying_type>(code)
                              | input_code_any_flag);
    }

    template<concepts::input_code T>
    [[nodiscard]] constexpr T remove_any_flag(T code) noexcept
    {
        return static_cast<T>(static_cast<input_code_underlying_type>(code)
                              | ~input_code_any_flag);
    }

    template<concepts::input_code T>
    [[nodiscard]] constexpr T to_any_code(T code) noexcept
    {
        return static_cast<T>((static_cast<input_code_underlying_type>(code) & input_code_flag<T>)
                              | input_code_any_flag);
    }

} // namespace detail

} // namespace flow
