#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <vector>

namespace Stardust_Celeste::Utilities::Input {

std::vector<Controller *> controller_map;

auto add_controller(Controller *controller) -> void {
    SC_CORE_DEBUG("Added Controller!");
    controller_map.push_back(controller);
}

auto clear_controller() -> void {
    SC_CORE_DEBUG("Cleared Controller(s)!");
    controller_map.clear();
}

auto update() -> void {
    for (auto &c : controller_map) {
        c->update();
    }
}

} // namespace Stardust_Celeste::Utilities::Input