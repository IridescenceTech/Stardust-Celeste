#include <Graphics/2D/FontRenderer.hpp>
#include <Stardust-Celeste.hpp>
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/PSPController.hpp>

using namespace Stardust_Celeste;
using namespace Stardust_Celeste::Utilities::Input;

class GameState : public Core::ApplicationState {

  public:
    void on_update(Core::Application *app, double dt) { update(); }
    void on_draw(Core::Application *app, double dt) { fontrenderer->draw(); }

    static void quit(std::any data) {
        auto gs = std::any_cast<GameState *>(data);

        SC_CORE_INFO("{}", gs->secret_value);
        exit(0);
    }

    void on_start() {
        texture = Rendering::TextureManager::get().load_texture(
            "default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST, true);

        fontrenderer = create_scopeptr<Graphics::G2D::FontRenderer>(
            texture, glm::vec2(16, 16));

        fontrenderer->add_text("HELLO!", {0, 0});

        Rendering::RenderContext::get().set_color({0, 0, 0, 255});
        Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, -30, 30);

        psp_controller = new PSPController();
        psp_controller->add_command(
            {static_cast<int>(PSPButtons::Start), Utilities::KeyFlag::Press},
            {quit, this});

        key_controller = new KeyboardController();
        key_controller->add_command(
            {static_cast<int>(Keys::Q), Utilities::KeyFlag::Press},
            {quit, this});

        add_controller(psp_controller);
        add_controller(key_controller);

        set_differential_mode("Mouse", true);
    }

    void on_cleanup() {
        Utilities::Input::clear_controller();
        delete psp_controller;
        delete key_controller;

        Rendering::TextureManager::get().delete_texture(texture);
    }

  private:
    int texture = 0;
    const int secret_value = 12;
    ScopePtr<Graphics::G2D::FontRenderer> fontrenderer;
    Utilities::Controller *psp_controller;
    Utilities::Controller *key_controller;
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