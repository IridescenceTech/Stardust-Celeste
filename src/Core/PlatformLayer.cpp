#include <Core/PlatformLayer.hpp>

namespace Stardust_Celeste::Core {

auto PlatformLayer::initialize(const AppConfig app) -> void {
  // TODO: Add layers as needed
  if(!app.headless) {
    Rendering::RenderContext::Get().initialize(app.render_settings);
  }
}

auto PlatformLayer::terminate() -> void {
  // TODO: Add layers as needed
  Rendering::RenderContext::Get().terminate();
}

} // namespace Stardust_Celeste::Core