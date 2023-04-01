#pragma once
#include "../Controller.hpp"

namespace Stardust_Celeste::Utilities::Input {

enum class VitaButtons {
    Select = 0x00000001,      //!< Select Button.
    L3 = 0x00000002,          //!< L3 Button.
    R3 = 0x00000004,          //!< R3 Button.
    Start = 0x00000008,       //!< Start Button.
    Up = 0x00000010,          //!< Up D-Pad Button.
    Right = 0x00000020,       //!< Right D-Pad Button.
    Down = 0x00000040,        //!< Down D-Pad Button.
    Left = 0x00000080,        //!< Left D-Pad Button.
    LTrigger = 0x00000100,    //!< Left Trigger.
    RTrigger = 0x00000200,    //!< Right Trigger.
    L1 = 0x00000400,          //!< L1 Button.
    R1 = 0x00000800,          //!< R1 Button.
    Triangle = 0x00001000,    //!< Triangle Button.
    Circle = 0x00002000,      //!< Circle Button.
    Cross = 0x00004000,       //!< Cross Button.
    Square = 0x00008000,      //!< Square Button.
    Intercepted = 0x00010000, //!< Input Not Available Because Intercepted By
    Headphone = 0x00080000,   //!< Headphone Plugged In.
    VolUp = 0x00100000,       //!< Volume Up Button.
    VolDown = 0x00200000,     //!< Volume Down Button.
    Power = 0x40000000        //!< Power Button.
};

class VitaController final : public Controller {
  public:
    VitaController();
    ~VitaController() = default;

    auto update() -> void;

    auto get_state(int key) -> bool;
};
} // namespace Stardust_Celeste::Utilities::Input