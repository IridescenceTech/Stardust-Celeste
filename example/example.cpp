#include <Graphics/2D/Tilemap.hpp>
#include <Stardust-Celeste.hpp>
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/PSPController.hpp>

using namespace Stardust_Celeste;
using namespace Stardust_Celeste::Utilities::Input;

class GameState : public Core::ApplicationState {

  public:
    void on_update(Core::Application *app, double dt) { update(); }
    void on_draw(Core::Application *app, double dt) { tilemap->draw(); }

    static void quit(std::any data) {
        auto gs = std::any_cast<GameState *>(data);

        SC_CORE_INFO("{}", gs->secret_value);
        exit(0);
    }

    void on_start() {
        texture = Rendering::TextureManager::get().load_texture(
            "container.jpg", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
            true);

        tilemap =
            create_scopeptr<Graphics::G2D::Tilemap>(texture, glm::vec2(4, 4));

        tilemap->add_tile(Graphics::G2D::Tile{
            {glm::vec2(0, 0), glm::vec2(0.5, 0.5)}, {255, 255, 255, 255}, 7});
        tilemap->add_tile(Graphics::G2D::Tile{
            {glm::vec2(0.5, 0), glm::vec2(0.5, 0.5)}, {255, 255, 255, 255}, 3});
        tilemap->add_tile(Graphics::G2D::Tile{
            {glm::vec2(0, 0.5), glm::vec2(0.5, 0.5)}, {255, 255, 255, 255}, 1});

        Rendering::RenderContext::get().matrix_ortho(-1, 1, -1, 1, -1, 1);

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
    ScopePtr<Graphics::G2D::Tilemap> tilemap;
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

    Core::PlatformLayer::get().initialize(config);

    return new GameApplication();
}