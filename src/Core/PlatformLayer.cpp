#include <Core/PlatformLayer.hpp>
#include <Network/NetworkDriver.hpp>
#include <Platform/Platform.hpp>
namespace Stardust_Celeste::Core {

auto PlatformLayer::initialize(const AppConfig app) -> void {
  // TODO: Add layers as needed
  if(!app.headless) {
    Rendering::RenderContext::Get().initialize(app.render_settings);

    #if BUILD_PLAT == BUILD_PSP
    if(app.networking)
      Network::NetworkDriver::Get().initGUI();
    #endif
  }

  auto net = &(Network::NetworkDriver::Get());
  if(app.networking && !net->isInit()) {
    net->init();
  }
}

auto PlatformLayer::terminate() -> void {
  Network::NetworkDriver::Get().cleanup();
  Rendering::RenderContext::Get().terminate();
}

} // namespace Stardust_Celeste::Core