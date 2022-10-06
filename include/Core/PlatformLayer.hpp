#pragma once
#include <Rendering/RenderContext.hpp>
#include <Utilities/Utilities.hpp>

namespace Stardust_Celeste::Core {

struct AppConfig {
    bool headless = false;
    Rendering::RenderContextSettings render_settings;
    bool networking = false;
};

struct PlatformLayer : Singleton {
    PlatformLayer() = default;

    auto initialize(const AppConfig app) -> void;
    auto terminate() -> void;

    inline static auto get() -> PlatformLayer & {
        static PlatformLayer ply;
        return ply;
    }
};
} // namespace Stardust_Celeste::Core