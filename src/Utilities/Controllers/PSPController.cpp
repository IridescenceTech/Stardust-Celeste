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

    for (const auto &[key, value] : command_map)
        if (!(oldPadData.Buttons & key) && currentPadData.Buttons & key)
            value.func(value.data);

#endif
}
} // namespace Stardust_Celeste::Utilities::Input