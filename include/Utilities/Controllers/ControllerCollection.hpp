#pragma once
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/MouseController.hpp>
#include <Utilities/Controllers/PSPController.hpp>
#include <Utilities/Controllers/VitaController.hpp>
#include <Utilities/Controllers/N3DSController.hpp>
#include <Utilities/Input.hpp>

namespace Stardust_Celeste::Utilities::Input {
struct ControllerCollection {

    inline static auto create_controller_collection() -> RefPtr<ControllerCollection> {
        auto res = create_refptr<ControllerCollection>();
        
        res->keyboard_controller = new KeyboardController();
        res->mouse_controller = new MouseController();
        res->psp_controller = new PSPController();
        res->vita_controller = new VitaController();
        res->n3ds_controller = new N3DSController();

        return res;
    }

    inline auto add_controller_collection() -> void {
        Input::add_controller(keyboard_controller);
        Input::add_controller(mouse_controller);
        Input::add_controller(psp_controller);
        Input::add_controller(vita_controller);
        Input::add_controller(n3ds_controller);
    }

    Controller *keyboard_controller;
    Controller *mouse_controller;
    Controller *psp_controller;
    Controller *vita_controller;
    Controller *n3ds_controller;
};
} // namespace Stardust_Celeste::Utilities::Input