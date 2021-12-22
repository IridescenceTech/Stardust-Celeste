#include <Stardust-Celeste.hpp>

using namespace Stardust_Celeste;


class GameApplication : public Core::Application {
public:
    GameApplication() {
        SC_APP_INFO("HELLO WORLD!");
        Application::Get().Exit();
    }

    ~GameApplication() = default;

private:

};

Core::Application* CreateNewSCApp() {
    Core::AppConfig config;
    config.headless = true;

    Core::PlatformLayer::Get().initialize(config);

    return new GameApplication();    
}