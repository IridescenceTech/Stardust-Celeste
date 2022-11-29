#include <Audio/AudioContext.hpp>
#include <Core/PlatformLayer.hpp>
#include <Network/NetworkDriver.hpp>
#include <Platform/Platform.hpp>

#if PSP
#include <psppower.h>
#endif

namespace Stardust_Celeste::Core {

auto PlatformLayer::initialize(const AppConfig app) -> void {
#if PSP
    scePowerSetClockFrequency(333, 333, 166);
#endif

    if (!app.headless) {
        Rendering::RenderContext::get().initialize(app.render_settings);
        Audio::AudioContext::get().initialize();
    }

    auto net = &(Network::NetworkDriver::get());
#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_VITA
    if (app.networking && !net->is_init()) {
        net->init();
    }
#endif
}

auto PlatformLayer::terminate() -> void {
    Network::NetworkDriver::get().cleanup();
    Rendering::RenderContext::get().terminate();
    Audio::AudioContext::get().terminate();
}

} // namespace Stardust_Celeste::Core