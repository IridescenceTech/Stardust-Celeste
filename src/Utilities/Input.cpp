#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <string>
#if USE_EASTL
#include <EASTL/vector.h>
#else
#include <vector>
#endif

#if PSP
#include <pspctrl.h>
#endif
#if VITA
#include <vitasdk.h>
#endif

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

#if USE_EASTL
eastl::vector<Controller *> controller_map;
#else
std::vector<Controller *> controller_map;
#endif

auto add_controller(Controller *controller) -> void {
    SC_CORE_DEBUG("Added Controller!");
    controller_map.push_back(controller);
}

auto clear_controller() -> void {
    SC_CORE_DEBUG("Cleared Controller(s)!");
    controller_map.clear();
}

#if PSP || VITA
extern SceCtrlData currentPadData;
#endif

bool diff_mode[5];

auto update() -> void {
    for (auto &c : controller_map) {
        c->update();
    }
}

auto set_cursor_center() -> void {
#if BUILD_PC
    if (diff_mode[0]) {
        // Reset to center - hide cursor
        int w, h;
        glfwGetWindowSize(GI::window, &w, &h);
        glfwSetCursorPos(GI::window, w / 2.0, h / 2.0);
    }
#endif
}

auto get_axis(std::string device, std::string axis) -> float {
    float res = 0.5f;

    auto devIDX = -1;

    if (device == "Mouse")
        devIDX = 0;
    else if (device == "PSP")
        devIDX = 1;
    else if (device == "Vita")
        devIDX = 2;
    else if (device == "3DS")
        devIDX = 3;

    if (devIDX == -1)
        return res;

    if (devIDX == 0) {
#if BUILD_PC
        double lx, ly;
        glfwGetCursorPos(GI::window, &lx, &ly);

        int w, h;
        glfwGetWindowSize(GI::window, &w, &h);

        if (axis == "X") {
            res = lx / (float)w;
        } else if (axis == "Y") {
            res = ly / (float)h;
        }
#endif
    } else if (devIDX == 1) {
#if PSP
        if (axis == "X") {
            res = (float)currentPadData.Lx / 255.0f;
        } else if (axis == "Y") {
            res = (float)currentPadData.Ly / 255.0f;
        }
#endif
    } else if (devIDX == 2) {
#if VITA
        if (axis == "LX") {
            res = (float)currentPadData.lx / 255.0f;
        } else if (axis == "LY") {
            res = (float)currentPadData.ly / 255.0f;
        } else if (axis == "RX") {
            res = (float)currentPadData.rx / 255.0f;
        } else if (axis == "RY") {
            res = (float)currentPadData.ry / 255.0f;
        }
#endif
    } else if (devIDX == 3) {
#if BUILD_PLAT == BUILD_3DS
        circlePosition cPos;
        hidCircleRead(&cPos);

        auto x = (float)cPos.dx / 154.0f;
        auto y = (float)cPos.dy / 154.0f;

        if (axis == "LX") {
            res = x;
        } else if (axis == "LY") {
            res = y;
        }

        res = res / 2.0f + 0.5f;
#endif
    }

    if (diff_mode[devIDX]) {
        res = (res - 0.5f) * 2.f;
    }

    return res;
}

auto set_differential_mode(std::string device, bool diff) -> void {
    if (device == "Mouse") {
#if BUILD_PC
        if (diff)
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

#endif
        diff_mode[0] = diff;
    } else if (device == "PSP") {
        diff_mode[1] = diff;
    } else if (device == "Vita") {
        diff_mode[2] = diff;
    } else if (device == "3DS") {
        diff_mode[3] = diff;
    }
}

} // namespace Stardust_Celeste::Utilities::Input