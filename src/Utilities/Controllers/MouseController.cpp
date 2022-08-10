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
    // Required to pass this pointer to GLFW
    MouseController *thizz;
    // Value that needs to be achieved before a scroll happens
    // (Useful for debouncing broken mice wheels and prevent overreaction)
    const int scroll_wheel_threshold = 2;
    int current_scroll_value = 0;

MouseController::MouseController()
{
    setup_scroll();
}

auto MouseController::update() -> void {
#if BUILD_PC
    for (const auto& [key, value] : command_map) {
        if (key.flags & KeyFlag::None) return;
        if (key.key >= 0 && key.key < 32) {

            mouseLast[key.key] = mouseNow[key.key];
            mouseNow[key.key] = glfwGetMouseButton(Rendering::window, key.key);

            if ((key.flags & KeyFlag::Press && !mouseLast[key.key] && mouseNow[key.key]) ||
                (key.flags & KeyFlag::Held && mouseLast[key.key] && mouseNow[key.key]) ||
                (key.flags & KeyFlag::Release && mouseLast[key.key] && !mouseNow[key.key]) ||
                (key.flags & KeyFlag::Untouched && !mouseLast[key.key] && !mouseNow[key.key]))
                value.func(value.data);
        }

    }
#endif
}

/** Tells GLFW to check command_map and call the registered commands accordingly */
auto MouseController::setup_scroll() -> void {
#if BUILD_PC
    thizz = this;

    glfwSetScrollCallback(Rendering::window,[](GLFWwindow* window, double x, double y) -> void {
        for(const auto& [key, value] : thizz->command_map) {
            // Only look for Press
            if (!(key.flags & KeyFlag::Press)) return;
            if ((key.key == (int)MouseButtons::MWheelUp) && (y >= 0)) {
                current_scroll_value += y;
                if (current_scroll_value > scroll_wheel_threshold) {
                    current_scroll_value = 0;
                    value.func(value.data);
                }
            }
            else if ((key.key == (int)MouseButtons::MWheelDown) && (y < 0)) {
                // Value is negative, so adding it will properly contribute towards the negative treshold
                current_scroll_value += y;
                if (current_scroll_value < -scroll_wheel_threshold) {
                    current_scroll_value = 0;
                    value.func(value.data);
                }
            }
        }
    });
#endif
}
} // namespace Stardust_Celeste::Utilities::Input