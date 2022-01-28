#pragma once
#include "../Controller.hpp"
namespace Stardust_Celeste::Utilities::Input {

enum class MouseButtons {
    Left = 0,
    Right = 1,
    Middle = 2,
    M4 = 3,
    M5 = 4,
    M6 = 5,
    M7 = 6,
    M8 = 7,
    Last = 8,
};

class MouseController final : public Controller {
  public:
    MouseController() = default;
    ~MouseController() = default;

    auto update() -> void;
};
} // namespace Stardust_Celeste::Utilities::Input