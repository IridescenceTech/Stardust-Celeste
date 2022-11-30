#pragma once
#include "../Controller.hpp"

namespace Stardust_Celeste::Utilities::Input {

#ifdef BIT
#undef BIT
#endif

#define BIT(n) (1 << (n))
enum class N3DSButtons {
    A = BIT(0),             ///< A
    B = BIT(1),             ///< B
    Select = BIT(2),        ///< Select
    Start = BIT(3),         ///< Start
    Dright = BIT(4),        ///< D-Pad Right
    Dleft = BIT(5),         ///< D-Pad Left
    Dup = BIT(6),           ///< D-Pad Up
    Ddown = BIT(7),         ///< D-Pad Down
    R = BIT(8),             ///< R
    L = BIT(9),             ///< L
    X = BIT(10),            ///< X
    Y = BIT(11),            ///< Y
    ZL = BIT(14),           ///< Zl (New 3ds Only)
    ZR = BIT(15),           ///< Zr (New 3ds Only)
    Touch = BIT(20),        ///< Touch (Not Actually Provided By Hid)
    CStick_Right = BIT(24), ///< C-Stick Right (New 3ds Only)
    CStick_Left = BIT(25),  ///< C-Stick Left (New 3ds Only)
    CStick_Up = BIT(26),    ///< C-Stick Up (New 3ds Only)
    CStick_Down = BIT(27),  ///< C-Stick Down (New 3ds Only)
    CPad_Right = BIT(28),   ///< Circle Pad Right
    CPad_Left = BIT(29),    ///< Circle Pad Left
    CPad_Up = BIT(30),      ///< Circle Pad Up
    CPad_Down = BIT(31),    ///< Circle Pad Down

    // Generic Catch-All Directions
    Up = Dup | CPad_Up,          ///< D-Pad Up Or Circle Pad Up
    Down = Ddown | CPad_Down,    ///< D-Pad Down Or Circle Pad Down
    Left = Dleft | CPad_Left,    ///< D-Pad Left Or Circle Pad Left
    Right = Dright | CPad_Right, ///< D-Pad Right Or Circle Pad Right
};

class N3DSController final : public Controller {
  public:
    N3DSController();
    ~N3DSController() = default;

    auto update() -> void;
};
} // namespace Stardust_Celeste::Utilities::Input