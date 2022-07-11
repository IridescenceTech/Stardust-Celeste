#include <Graphics/2D/FontRenderer.hpp>
#include <Stardust-Celeste.hpp>
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/PSPController.hpp>

using namespace Stardust_Celeste;
using namespace Stardust_Celeste::Utilities::Input;

class GameState : public Core::ApplicationState {

  public:
    void on_update(Core::Application *app, double dt) {}
    void on_draw(Core::Application *app, double dt) {}

    void on_start() { SC_APP_INFO("HELLO WORLD!"); }

    void on_cleanup() {}

  private:
};

class GameApplication : public Core::Application {
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
    config.render_settings.width = 960;
    config.render_settings.height = 544;

    Core::PlatformLayer::get().initialize(config);

    return new GameApplication();
}