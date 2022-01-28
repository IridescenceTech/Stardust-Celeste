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

/**
 * @brief Gets the value of an axis from analog from a device
 *
 * @param device Device Name
 * @param axis Axis to get
 * @return float Axis value normalized from [0, 1] where the center is 0.5 -
 * returns in [-1, 1] if differential
 */
auto get_axis(std::string device, std::string axis) -> float;

/**
 * @brief Set the differential mode for an analog device
 *
 * @param device Device Name
 * @param diff Axis to get
 */
auto set_differential_mode(std::string device, bool diff) -> void;

/**
 * @brief Set the cursor to the center (PC Only)
 *
 */
auto set_cursor_center() -> void;

} // namespace Stardust_Celeste::Utilities::Input