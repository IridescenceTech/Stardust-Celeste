#pragma once
#include "Controller.hpp"
namespace Stardust_Celeste::Utilities::Input {

/**
 * @brief Adds a controller to track
 *
 * @param controller Controller to track
 */
auto add_controller(Controller *controller) -> void;

/**
 * @brief Clears all tracked controllers
 *
 */
auto clear_controller() -> void;

/**
 * @brief Updates all given controllers
 *
 */
auto update() -> void;

} // namespace Stardust_Celeste::Utilities::Input