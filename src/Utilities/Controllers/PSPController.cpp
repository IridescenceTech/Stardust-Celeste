#include <Utilities/Controllers/PSPController.hpp>

#if PSP
#include <pspctrl.h>
#endif

namespace Stardust_Celeste::Utilities::Input {

#if PSP
SceCtrlData currentPadData;
SceCtrlData oldPadData;
#endif

PSPController::PSPController() {
#if PSP
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    sceCtrlReadBufferPositive(&currentPadData, 1);
#endif
}
auto PSPController::update() -> void {
#if PSP
    oldPadData = currentPadData;
    sceCtrlReadBufferPositive(&currentPadData, 1);

    for (const auto& [key, value] : command_map) {
        if (key.flags & KeyFlag::None) return;
        if (key.key < 0 && key.key >= 1024) return;

        auto keysLast = oldPadData.Buttons & key.key;
        auto keysNow = currentPadData.Buttons & key.key;

        if ((key.flags & KeyFlag::Press && !keysLast && keysNow) ||
            (key.flags & KeyFlag::Held && keysLast && keysNow) ||
            (key.flags & KeyFlag::Release && keysLast && !keysNow) ||
            (key.flags & KeyFlag::Untouched && !keysLast && !keysNow))
            value.func(value.data);
    }

#endif
}
} // namespace Stardust_Celeste::Utilities::Input