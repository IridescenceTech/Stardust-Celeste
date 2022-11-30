#include <Utilities/Controllers/N3DSController.hpp>

#if BUILD_PLAT == BUILD_3DS
#include <3ds.h>
#endif

namespace Stardust_Celeste::Utilities::Input {

#if BUILD_PLAT == BUILD_3DS
u32 current;
u32 last;
#endif

N3DSController::N3DSController() {}
auto N3DSController::update() -> void {
#if BUILD_PLAT == BUILD_3DS
    hidScanInput();

    last = current;
    current |= hidKeysDown();
    current ^= hidKeysUp();

    for (const auto &[key, value] : command_map) {
        if (key.flags & KeyFlag::None)
            return;
        if (key.key < 0 && key.key >= 1024)
            return;

        auto keysLast = last & key.key;
        auto keysNow = current & key.key;

        if ((key.flags & KeyFlag::Press && !keysLast && keysNow) ||
            (key.flags & KeyFlag::Held && keysLast && keysNow) ||
            (key.flags & KeyFlag::Release && keysLast && !keysNow) ||
            (key.flags & KeyFlag::Untouched && !keysLast && !keysNow))
            value.func(value.data);
    }

#endif
}
} // namespace Stardust_Celeste::Utilities::Input