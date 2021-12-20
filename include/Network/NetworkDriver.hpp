/**
 * @file NetworkDriver.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Network driver for all platforms
 * @version 0.1
 * @date 2021-12-20
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
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

  /**
   * @brief Cleans up the network code
   */
  static auto cleanup() -> void;

  /**
   * @brief Checks if the network driver is already set up
   * @return - If the network driver has been set up
   */
  inline auto isInit() const -> bool { return isInitialized; }

  /**
   * @brief Gets the instance of the net driver
   * @return - Instance of this
   */
  static NetworkDriver &Get() {
    static NetworkDriver instance;
    return instance;
  }

private:
  bool isInitialized = false;
};
} // namespace Stardust_Celeste::Network