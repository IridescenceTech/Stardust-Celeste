#pragma once
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Input.hpp>

namespace Stardust_Celeste::Utilities::Input {
struct ControllerCollection {

    inline auto add_controller_collection() -> void {
        Input::add_controller(keyboard_controller);
        Input::add_controller(mouse_controller);
        Input::add_controller(psp_controller);
        Input::add_controller(vita_controller);
    }

    Controller *keyboard_controller;
    Controller *mouse_controller;
    Controller *psp_controller;
    Controller *vita_controller;
};
} // namespace Stardust_Celeste::Utilities::Input