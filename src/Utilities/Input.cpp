#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// TODO: LOAD CONFIGURATION FROM JSON
#define ACTION_FORWARD GLFW_KEY_W
#define ACTION_BACKWARD GLFW_KEY_S
#define ACTION_LEFT GLFW_KEY_A
#define ACTION_RIGHT GLFW_KEY_D

#define ACTION_PRIMARY GLFW_MOUSE_BUTTON_1
#define ACTION_ALTERNATE GLFW_MOUSE_BUTTON_2

#elif BUILD_PLAT == BUILD_PSP
#include <pspctrl.h>

#define ACTION_FORWARD PSP_CTRL_TRIANGLE
#define ACTION_BACKWARD PSP_CTRL_CROSS
#define ACTION_LEFT PSP_CTRL_SQUARE
#define ACTION_RIGHT PSP_CTRL_CIRCLE

#define ACTION_PRIMARY PSP_CTRL_LTRIGGER
#define ACTION_ALTERNATE PSP_CTRL_RTRIGGER
#endif

#if BUILD_PC
namespace Stardust_Celeste::Rendering {
extern GLFWwindow *window;
}
#endif

namespace Stardust_Celeste::Utilities::Input {

#if BUILD_PLAT == BUILD_PSP
SceCtrlData padData;
#endif

auto update_input() -> void {
#if BUILD_PLAT == BUILD_PSP
    sceCtrlReadBufferPositive(&padData, 1);
#endif
    // PC is handled elsewhere
}

auto check_action(int num, bool alt = false) -> bool {
#if BUILD_PC
    if (alt)
        return glfwGetMouseButton(Rendering::window, num) == GLFW_PRESS;
    else
        return glfwGetKey(Rendering::window, num) == GLFW_PRESS;
#elif BUILD_PSP
    return (padData.Buttons & num);
#endif
}

auto get_action_state(Action action) -> bool {
    switch (action) {
    case Action::Forward:
        return check_action(ACTION_FORWARD);
    case Action::Backward:
        return check_action(ACTION_BACKWARD);
    case Action::Left:
        return check_action(ACTION_LEFT);
    case Action::Right:
        return check_action(ACTION_RIGHT);

    case Action::Primary:
        return check_action(ACTION_PRIMARY, true);
    case Action::Alternate:
        return check_action(ACTION_ALTERNATE, true);

    default:
        return false;
    }
}

auto get_cursor_pos(float &x, float &y) -> void {
#if BUILD_PC
    double lx, ly;
    glfwGetCursorPos(Rendering::window, &lx, &ly);

    int w, h;
    glfwGetWindowSize(Rendering::window, &w, &h);

    x = lx / (float)w;
    y = ly / (float)h;

#elif BUILD_PSP
    x = (float)padData.Lx / 255.0f;
    y = (float)padData.Ly / 255.0f;
#endif
}
} // namespace Stardust_Celeste::Utilities::Input