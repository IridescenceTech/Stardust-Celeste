#include <Stardust-Celeste.hpp>
#include <Lua/LuaContext.hpp>
#include <Lua/LuaScript.hpp>

using namespace Stardust_Celeste;
using namespace Stardust_Celeste::Utilities::Input;

class GameState : public Core::ApplicationState {

public:
    GameState() {}

    void on_update(Core::Application *app, double dt) {
        l->update();
    }

    void on_draw(Core::Application *app, double dt) {
        l->draw();
    }

    void on_start() {
        Scripting::LuaContext::get().init();

        l = new Stardust_Celeste::Scripting::LuaScript("./script.lua");
        l->run();
    }

    void on_cleanup() {
        delete l;
    }

private:
    Scripting::LuaScript* l;
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