#pragma once
#include <Rendering/RenderContext.hpp>
#include <Utilities/Utilities.hpp>

namespace Stardust_Celeste::Core {

/**
 * @brief Application Configuration
 * Headless mode -- no graphical UI 
 * Render Settings -- settings for renderer (may be platform specific)
 * Networking -- whether or not networking is enabled at launch
 */
struct AppConfig {
    bool headless = false;
    Rendering::RenderContextSettings render_settings;
    bool networking = false;
};

/**
 * @brief Platform Layer initialization
 * 
 */
struct PlatformLayer : Singleton {
    PlatformLayer() = default;

    /**
     * @brief Initialize with given configuration settings
     * 
     * @param config Application Configuration
     */
    auto initialize(const AppConfig config) -> void;

    /**
     * @brief Terminate the platform layer
     * 
     */
    auto terminate() -> void;

    /**
     * @brief Get Singleton Instance of the PlatformLayer class
     * 
     * @return PlatformLayer& Instance
     */
    inline static auto get() -> PlatformLayer & {
        static PlatformLayer ply;
        return ply;
    }
};
} // namespace Stardust_Celeste::Core