#pragma once
#include <vector>

#include "../Rendering/Rendering.hpp"
#include "../Utilities/Utilities.hpp"
#include "State.hpp"

/**
 * @brief Predeclared main
 */
int main(int argc, char **argv);

namespace Stardust_Celeste::Core {

/**
 * @brief Base application class -- must be overriden for users
 * 
 */
class Application {
  public:
    Application() : running(true), frameTime(0.0f) {
        SC_CORE_ASSERT(!s_Instance, "Instance shouldn't exist!");
        s_Instance = this;
    }

    virtual ~Application() = default;

    static Application &get() { return *s_Instance; }

    /**
     * @brief Set the state
     * 
     * @param state Valid ApplicationState
     */
    void set_state(RefPtr<ApplicationState> state) {
        SC_CORE_ASSERT(state.get() == nullptr, "State set was nullptr!");

        stateStack.clear();
        state->on_start();
        stateStack.emplace_back(state);
        stateStack.shrink_to_fit();
    }

    /**
     * @brief Push the state
     * 
     * @param state Valid ApplicationState
     */
    void push_state(RefPtr<ApplicationState> state) {
        SC_CORE_ASSERT(state.get() == nullptr, "State pushed was nullptr!");
        state->on_start();
        stateStack.emplace_back(state);
    }

    /**
     * @brief Pops the state
     * 
     */
    void pop_state() {
        SC_CORE_ASSERT(stateStack.size() > 0, "StateStack.size() == 0, but pop_state was called!");
        stateStack.back()->on_cleanup();
        stateStack.pop_back();
    }

    virtual void on_start() = 0;

    /**
     * @brief Exits the application
     * 
     */
    void exit() { running = false; }

  private:
    /**
     * @brief Runs the base application
     * 
     */
    void run() {
        Utilities::Timer timer;

        while (running) {
            frameTime = static_cast<float>(timer.get_delta_time());

            if (!stateStack.empty()) {
                stateStack.back()->on_update(this, frameTime);

                auto rctx = Rendering::RenderContext::get().initialized();

                if (rctx)
                    Rendering::RenderContext::get().clear();

                stateStack.back()->on_draw(this, frameTime);

                if (rctx)
                    Rendering::RenderContext::get().render();
            }
        }
    }

    static Application *s_Instance;

    bool running;
    double frameTime;

    std::vector<RefPtr<ApplicationState>> stateStack;

    friend int ::main(int argc, char **argv);
};
} // namespace Stardust_Celeste::Core
