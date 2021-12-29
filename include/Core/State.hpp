/**
 * @file State.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief State
 * @version 0.1
 * @date 2021-12-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

namespace Stardust_Celeste::Core {
class Application;

class ApplicationState {
public:
  ApplicationState() = default;

  ~ApplicationState() = default;

  virtual void onUpdate(Application *app, double dt) = 0;
  virtual void onDraw(Application *app, double dt) = 0;

  // Note: Cannot use = default because virtual

  virtual void onStart() = 0;
  virtual void onCleanup() = 0;
};
} // namespace Stardust_Celeste::Core
