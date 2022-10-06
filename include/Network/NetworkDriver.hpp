#pragma once
#include <Platform/Platform.hpp>
#include <Utilities/Singleton.hpp>

namespace Stardust_Celeste::Network {

class NetworkDriver : public Singleton {
  public:
    NetworkDriver() = default;

    /**
     * @brief Starts up your networking code
     * @return - successful?
     */
    static auto init() -> bool;

#if BUILD_PLAT == BUILD_PSP
    static auto initGUI() -> bool;
#endif

    /**
     * @brief Cleans up the network code
     */
    static auto cleanup() -> void;

    /**
     * @brief Checks if the network driver is already set up
     * @return - If the network driver has been set up
     */
    inline auto is_init() const -> bool { return isInitialized; }

    /**
     * @brief Gets the instance of the net driver
     * @return - Instance of this
     */
    static NetworkDriver &get() {
        static NetworkDriver instance;
        return instance;
    }

  private:
    bool isInitialized = false;
};
} // namespace Stardust_Celeste::Network