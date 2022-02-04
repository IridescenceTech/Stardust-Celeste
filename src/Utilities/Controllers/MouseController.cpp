#include <Utilities/Controllers/MouseController.hpp>
#include <Utilities/Logger.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#if BUILD_PC
namespace Stardust_Celeste::Rendering {
extern GLFWwindow *window;
}
#endif

namespace Stardust_Celeste::Utilities::Input {

    bool keysLast[32];
    bool keysNow[32];

auto MouseController::update() -> void {
#if BUILD_PC
    for (const auto& [key, value] : command_map) {
        if (key.flags & KeyFlag::None) return;
        if (key.key < 0 && key.key >= 1024) return;

        keysLast[key.key] = keysNow[key.key];
        keysNow[key.key] = glfwGetMouseButton(Rendering::window, key.key);

        if ((key.flags & KeyFlag::Press && !keysLast[key.key] && keysNow[key.key]) ||
            (key.flags & KeyFlag::Held && keysLast[key.key] && keysNow[key.key]) ||
            (key.flags & KeyFlag::Release && keysLast[key.key] && !keysNow[key.key]) ||
            (key.flags & KeyFlag::Untouched && !keysLast[key.key] && !keysNow[key.key]))
            value.func(value.data);

    }
#endif
}
} // namespace Stardust_Celeste::Utilities::Input