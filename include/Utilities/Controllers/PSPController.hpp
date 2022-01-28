#pragma once
#include "../Controller.hpp"

namespace Stardust_Celeste::Utilities::Input {

enum class PSPButtons {
    Select = 0x000001,
    Start = 0x000008,
    Up = 0x000010,
    Right = 0x000020,
    Down = 0x000040,
    Left = 0x000080,
    LTrigger = 0x000100,
    RTrigger = 0x000200,
    Triangle = 0x001000,
    Circle = 0x002000,
    Cross = 0x004000,
    Square = 0x008000,
    Home = 0x010000,
    Hold = 0x020000,
    Note = 0x800000,
    Screen = 0x400000,
    VolUp = 0x100000,
    VolDown = 0x200000,
    Wlan_Up = 0x040000,
    Remote = 0x080000,
    Disc = 0x1000000,
    MS = 0x2000000,
};

class PSPController final : public Controller {
  public:
    PSPController();
    ~PSPController() = default;

    auto update() -> void;
};
} // namespace Stardust_Celeste::Utilities::Input