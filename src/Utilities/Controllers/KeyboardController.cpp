#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Logger.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#if BUILD_PC
namespace GI {
extern GLFWwindow *window;
}
#endif

namespace Stardust_Celeste::Utilities::Input {
bool keysLast[1024];
bool keysNow[1024];
auto KeyboardController::update() -> void {
#if BUILD_PC
    for (const auto &pair : command_map) {
        KeyData key = pair.key;
        Command value = pair.cmd;

        if (key.flags & KeyFlag::None)
            return;
        if (key.key >= 0 && key.key < 1024) {

            keysLast[key.key] = keysNow[key.key];
            keysNow[key.key] = glfwGetKey(GI::window, key.key);

            if ((key.flags & KeyFlag::Press && !keysLast[key.key] &&
                 keysNow[key.key]) ||
                (key.flags & KeyFlag::Held && keysLast[key.key] &&
                 keysNow[key.key]) ||
                (key.flags & KeyFlag::Release && keysLast[key.key] &&
                 !keysNow[key.key]) ||
                (key.flags & KeyFlag::Untouched && !keysLast[key.key] &&
                 !keysNow[key.key]))
                value.func(value.data);
        }
    }

#endif
}

auto KeyboardController::get_state(int key) -> bool {
    return keysNow[key];
}
} // namespace Stardust_Celeste::Utilities::Input