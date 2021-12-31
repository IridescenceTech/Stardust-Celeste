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
#include <vector>

#include "../Rendering/Rendering.hpp"
#include "../Utilities/Utilities.hpp"
#include "State.hpp"

int main(int argc, char **argv);

namespace Stardust_Celeste::Core {
class Application {
  public:
    Application() : running(true), frameTime(0.0f) {
        SC_CORE_ASSERT(!s_Instance, "Instance shouldn't exist!");
        s_Instance = this;
    }

    ~Application() = default;

    static Application &get() { return *s_Instance; }

    void set_state(RefPtr<ApplicationState> state) {
        stateStack.clear();
        state->on_start();
        stateStack.emplace_back(state);
        stateStack.shrink_to_fit();
    }

    void push_state(RefPtr<ApplicationState> state) {
        state->on_start();
        stateStack.emplace_back(state);
    }

    void pop_state() {
        stateStack.back()->on_cleanup();
        stateStack.pop_back();
    }

    virtual void on_start() = 0;

    void exit() { running = false; }

  private:
    void run() {
        Utilities::Timer timer;

        while (running) {
            frameTime = static_cast<float>(timer.get_delta_time());

            auto rctx = Rendering::RenderContext::get().initialized();

            if (rctx)
                Rendering::RenderContext::get().clear();

            if (!stateStack.empty()) {
                stateStack.back()->on_update(this, frameTime);
                stateStack.back()->on_draw(this, frameTime);
            }

            if (rctx)
                Rendering::RenderContext::get().render();
        }
    }

    static Application *s_Instance;

    bool running;
    double frameTime;

    std::vector<RefPtr<ApplicationState>> stateStack;

    friend int ::main(int argc, char **argv);
};
} // namespace Stardust_Celeste::Core
