#include <Core/Application.hpp>
#include <Utilities/Utilities.hpp>

using namespace Stardust_Celeste::Utilities;

auto init_sc() -> void {
    Logger::init();
    Timer::init();

    SC_CORE_INFO("Initialized base layer!");
}

auto cleanup_sc() -> void {
    Timer::cleanup();
    Logger::cleanup();
}

extern Stardust_Celeste::Core::Application* CreateNewSCApp();

int main(int, char**) {

    SC_PROFILE_BEGIN_SESSION("Init", "SC-Init.json");
    SC_PROFILE_FUNCTION(init_sc, __LINE__, __FILE__);
    
    auto app = CreateNewSCApp();
    
    SC_PROFILE_END_SESSION();

    
    SC_PROFILE_BEGIN_SESSION("User Session", "SC-User.json");
    app->Run();
    SC_PROFILE_END_SESSION();

    SC_PROFILE_BEGIN_SESSION("Cleanup", "SC-Cleanup.json");
    
    delete app;

    SC_PROFILE_FUNCTION(cleanup_sc, __LINE__, __FILE__)
    SC_PROFILE_END_SESSION();

    return EXIT_SUCCESS;
}