#include <Stardust-Celeste.hpp>

using namespace Stardust_Celeste;


class GameState : public Core::ApplicationState {
    public:

    void onUpdate(Core::Application *app, double dt) {

    }
    void onDraw(Core::Application *app, double dt) {
        mesh->draw();   
    }

    void onStart() {
        auto vertex_array = new Rendering::Vertex[4];
        vertex_array[0] = Rendering::Vertex{0.f, 0.f, {0xff, 0x00, 0x00, 0xff}, 0.f, 0.f, 0.f};
        vertex_array[1] = Rendering::Vertex{0.f, 0.f, {0x00, 0xff, 0x00, 0xff}, 1.f, 0.f, 0.f};
        vertex_array[2] = Rendering::Vertex{0.f, 0.f, {0x00, 0x00, 0xff, 0xff}, 1.f, 1.f, 0.f};
        vertex_array[3] = Rendering::Vertex{0.f, 0.f, {0xff, 0xff, 0xff, 0xff}, 0.f, 1.f, 0.f};

        auto idx_array = new u16[6];
        idx_array[0] = 0;
        idx_array[1] = 1;
        idx_array[2] = 2;
        idx_array[3] = 2;
        idx_array[4] = 3;
        idx_array[5] = 0;

        mesh = createScopePtr<Rendering::Mesh>(vertex_array, idx_array, 6);

        #if PSP
        sceKernelDcacheWritebackInvalidateAll();
        #endif
    }
    void onCleanup() {
        
    }

    private:
    ScopePtr<Rendering::Mesh> mesh;
};

class GameApplication : public Core::Application {
public:
    GameApplication() {
        state = createRefPtr<GameState>();
    }

    ~GameApplication() = default;

    void OnStart() override {
        Application::Get().PushState(state);
    }

private:
    RefPtr<GameState> state;
};

Core::Application* CreateNewSCApp() {
    Core::AppConfig config;
    config.headless = false;

    Core::PlatformLayer::Get().initialize(config);

    return new GameApplication();    
}