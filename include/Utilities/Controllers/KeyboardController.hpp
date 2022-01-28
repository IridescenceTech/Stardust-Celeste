#pragma once
#include "../Controller.hpp"
namespace Stardust_Celeste::Utilities::Input {

enum class Keys {
    Space = 32,
    Apostrophe = 39, /* ' */
    Comma = 44,      /* , */
    Minus = 45,      /* - */
    Period = 46,     /* . */
    Slash = 47,      /* / */
    Num0 = 48,
    Num1 = 49,
    Num2 = 50,
    Num3 = 51,
    Num4 = 52,
    Num5 = 53,
    Num6 = 54,
    Num7 = 55,
    Num8 = 56,
    Num9 = 57,
    Semicolon = 59, /* ; */
    Equal = 61,     /* = */
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LBracket = 91,     /* [ */
    Backslash = 92,    /* \ */
    RBracket = 93,     /* ] */
    Grave_Accent = 96, /* ` */
    World_1 = 161,     /* non-US #1 */
    World_2 = 162,     /* non-US #2 */
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    Page_Up = 266,
    Page_Down = 267,
    Home = 268,
    End = 269,
    Caps_Lock = 280,
    Scroll_Lock = 281,
    Num_Lock = 282,
    Print_Screen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    KeyPad0 = 320,
    KeyPad1 = 321,
    KeyPad2 = 322,
    KeyPad3 = 323,
    KeyPad4 = 324,
    KeyPad5 = 325,
    KeyPad6 = 326,
    KeyPad7 = 327,
    KeyPad8 = 328,
    KeyPad9 = 329,
    KeyPadDecimal = 330,
    KeyPadDivide = 331,
    KeyPadMultiply = 332,
    KeyPadSubtract = 333,
    KeyPadAdd = 334,
    KeyPadEnter = 335,
    KeyPadEqual = 336,
    LShift = 340,
    LControl = 341,
    LAlt = 342,
    LSuper = 343,
    RShift = 344,
    RControl = 345,
    RAlt = 346,
    RSuper = 347,
    Menu = 348,
};

class KeyboardController final : public Controller {
  public:
    KeyboardController() = default;
    ~KeyboardController() = default;

    auto update() -> void;
};
} // namespace Stardust_Celeste::Utilities::Input