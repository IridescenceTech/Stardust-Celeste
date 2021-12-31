#include <Stardust-Celeste.hpp>

using namespace Stardust_Celeste;

class GameState : public Core::ApplicationState {
  public:
    void on_update(Core::Application *app, double dt) {}
    void on_draw(Core::Application *app, double dt) {
        Rendering::TextureManager::get().bind_texture(texture);
        mesh->draw();
    }

    void on_start() {
        auto vertex_array = new Rendering::Vertex[4];
        vertex_array[0] = Rendering::Vertex{0.f, 0.f, {0xff, 0x00, 0x00, 0xff},
                                            0.f, 0.f, 0.f};
        vertex_array[1] = Rendering::Vertex{1.f, 0.f, {0x00, 0xff, 0x00, 0xff},
                                            1.f, 0.f, 0.f};
        vertex_array[2] = Rendering::Vertex{1.f, 1.f, {0x00, 0x00, 0xff, 0xff},
                                            1.f, 1.f, 0.f};
        vertex_array[3] = Rendering::Vertex{0.f, 1.f, {0xff, 0xff, 0xff, 0xff},
                                            0.f, 1.f, 0.f};

        auto idx_array = new u16[6];
        idx_array[0] = 0;
        idx_array[1] = 1;
        idx_array[2] = 2;
        idx_array[3] = 2;
        idx_array[4] = 3;
        idx_array[5] = 0;

        mesh = create_scopeptr<Rendering::Mesh>(vertex_array, idx_array, 6);

        texture = Rendering::TextureManager::get().load_texture(
            "container.jpg", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
            true);

        Rendering::RenderContext::get().matrix_ortho(-1, 1, -1, 1, -1, 1);
    }

    void on_cleanup() {}

  private:
    int texture = 0;
    ScopePtr<Rendering::Mesh> mesh;
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