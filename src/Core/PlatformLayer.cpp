#include <Core/PlatformLayer.hpp>
#include <Network/NetworkDriver.hpp>
#include <Platform/Platform.hpp>
namespace Stardust_Celeste::Core {

auto PlatformLayer::initialize(const AppConfig app) -> void {
    // TODO: Add layers as needed
    if (!app.headless) {
        Rendering::RenderContext::get().initialize(app.render_settings);

#if BUILD_PLAT == BUILD_PSP
        if (app.networking)
            Network::NetworkDriver::get().initGUI();
#endif
    }

    auto net = &(Network::NetworkDriver::get());
    if (app.networking && !net->is_init()) {
        net->init();
    }
}

auto PlatformLayer::terminate() -> void {
    Network::NetworkDriver::get().cleanup();
    Rendering::RenderContext::get().terminate();
}

} // namespace Stardust_Celeste::Core