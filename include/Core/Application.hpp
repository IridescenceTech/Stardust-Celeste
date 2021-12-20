/**
 * @file Application.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Application definition
 * @version 0.1
 * @date 2021-12-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#include "../Utilities/Utilities.hpp"
#include "State.hpp"
#include <vector>

int main(int argc, char** argv);

namespace Stardust_Celeste::Core {
class Application {
public:
  Application() : running(true), frameTime(0.0f) {
    SC_CORE_ASSERT(!s_Instance, "Instance shouldn't exist!");
    s_Instance = this;
  }

  ~Application() = default;

  static Application &Get() { return *s_Instance; }

  void SetState(RefPtr<ApplicationState> state) {
    stateStack.clear();
    stateStack.emplace_back(state);
    stateStack.shrink_to_fit();
  }

  void PushState(RefPtr<ApplicationState> state) {
    stateStack.emplace_back(state);
  }

  void PopState() { stateStack.pop_back(); }

  void Exit() { running = false; }

private:
  void Run() {
    Utilities::Timer timer;

    while (running) {
      frameTime = static_cast<float>(timer.getDeltaTime());

      if (!stateStack.empty()) {
        stateStack.back()->onUpdate(this, frameTime);
        stateStack.back()->onDraw(this, frameTime);
      }
    }
  }

  static Application *s_Instance;

  bool running;
  bool frameTime;

  std::vector<RefPtr<ApplicationState>> stateStack;

  friend int ::main(int argc, char** argv);
};
} // namespace Stardust_Celeste::Core
