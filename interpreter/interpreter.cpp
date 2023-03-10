#include <Stardust-Celeste.hpp>
#include <Lua/LuaContext.hpp>

using namespace Stardust_Celeste;
using namespace Stardust_Celeste::Utilities::Input;

class GameState : public Core::ApplicationState {

public:
    GameState() {}

    void on_update(Core::Application *app, double dt) {}
    void on_draw(Core::Application *app, double dt) {}
    void on_start() {
        Scripting::LuaContext::get().init();

    }
    void on_cleanup() {}

private:
};


class LuaApplication : public Core::Application {
public:
    void on_start() override {
        state = create_refptr<GameState>();
        Application::get().push_state(state);
    }

private:
    RefPtr<GameState> state;
};


Core::Application *CreateNewSCApp() {
    Core::AppConfig config;
    config.headless = false;

    Core::PlatformLayer::get().initialize(config);

    return new LuaApplication();
}