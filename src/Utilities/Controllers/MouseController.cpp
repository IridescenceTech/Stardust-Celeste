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

auto MouseController::update() -> void {
#if BUILD_PC
    for (const auto &[key, value] : command_map)
        if (glfwGetMouseButton(Rendering::window, key) == GLFW_PRESS)
            value.func(value.data);

#endif
}
} // namespace Stardust_Celeste::Utilities::Input