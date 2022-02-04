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

    bool mouseLast[32];
    bool mouseNow[32];

auto MouseController::update() -> void {
#if BUILD_PC
    for (const auto& [key, value] : command_map) {
        if (key.flags & KeyFlag::None) return;
        if (key.key < 0 && key.key >= 1024) return;

        mouseLast[key.key] = mouseNow[key.key];
        mouseNow[key.key] = glfwGetMouseButton(Rendering::window, key.key);

        if ((key.flags & KeyFlag::Press && !mouseLast[key.key] && mouseNow[key.key]) ||
            (key.flags & KeyFlag::Held && mouseLast[key.key] && mouseNow[key.key]) ||
            (key.flags & KeyFlag::Release && mouseLast[key.key] && !mouseNow[key.key]) ||
            (key.flags & KeyFlag::Untouched && !mouseLast[key.key] && !mouseNow[key.key]))
            value.func(value.data);

    }
#endif
}
} // namespace Stardust_Celeste::Utilities::Input