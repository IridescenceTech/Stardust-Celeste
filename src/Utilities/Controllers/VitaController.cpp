#include <Utilities/Controllers/VitaController.hpp>

#if VITA
#include <vitasdk.h>
#endif

namespace Stardust_Celeste::Utilities::Input {

#if VITA
SceCtrlData currentPadData;
SceCtrlData oldPadData;
#endif

VitaController::VitaController() {
#if VITA
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceCtrlReadBufferPositive(0, &currentPadData, 1);
#endif
}
auto VitaController::update() -> void {
#if VITA
    oldPadData = currentPadData;
    sceCtrlReadBufferPositive(0, &currentPadData, 1);

    for (const auto &[key, value] : command_map) {
        if (key.flags & KeyFlag::None)
            return;
        if (key.key < 0 && key.key >= 1024)
            return;

        auto keysLast = oldPadData.buttons & key.key;
        auto keysNow = currentPadData.buttons & key.key;

        if ((key.flags & KeyFlag::Press && !keysLast && keysNow) ||
            (key.flags & KeyFlag::Held && keysLast && keysNow) ||
            (key.flags & KeyFlag::Release && keysLast && !keysNow) ||
            (key.flags & KeyFlag::Untouched && !keysLast && !keysNow))
            value.func(value.data);
    }

#endif
}
} // namespace Stardust_Celeste::Utilities::Input