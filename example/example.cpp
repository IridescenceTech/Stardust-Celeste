#include <Stardust-Celeste.hpp>

using namespace Stardust_Celeste;


class GameState : public Core::ApplicationState {
    public:

    void onUpdate(Core::Application *app, double dt) {

    }
    void onDraw(Core::Application *app, double dt) {
        
    }

    void onStart() {

    }
    void onCleanup() {

    }
};

class GameApplication : public Core::Application {
public:
    GameApplication() {
        SC_APP_INFO("HELLO WORLD!");

        state = createRefPtr<GameState>();
        this->PushState(state);
    }

    ~GameApplication() = default;

private:
    RefPtr<GameState> state;
};

Core::Application* CreateNewSCApp() {
    Core::AppConfig config;
    config.headless = false;

    Core::PlatformLayer::Get().initialize(config);

    return new GameApplication();    
}