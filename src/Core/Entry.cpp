#include <Core/Core.hpp>
#include <Platform/Platform.hpp>
#include <Utilities/Utilities.hpp>

#if PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Stardust-Celeste-App", 0, 1, 0);
auto cleanup_sc() -> void;
int exit_callback(int arg1, int arg2, void *common) {
    Stardust_Celeste::Core::PlatformLayer::get().terminate();
    cleanup_sc();
    sceKernelExitGame();
    return 0;
}

int CallbackThread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();

    return 0;
}

int SetupCallbacks(void) {
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11,
                                     0xFA0, 0, 0);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}

#endif

#if BUILD_PLAT == BUILD_VITA
#include <vitasdk.h>
#endif

#if BUILD_PLAT == BUILD_3DS
#include <3ds.h>
#endif

using namespace Stardust_Celeste::Utilities;

auto init_sc() -> void {
#if BUILD_PLAT == BUILD_3DS
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
#endif

    Logger::init();
    Timer::init();
    SC_CORE_INFO("Initialized base layer!");
}

auto cleanup_sc() -> void {
#if BUILD_PLAT == BUILD_3DS
    gfxExit();
#endif

    Timer::cleanup();
    Logger::cleanup();
}

extern Stardust_Celeste::Core::Application *CreateNewSCApp();
int main(int, char **) {

#if PSP
    SetupCallbacks();
    // Access full texture range
    sceGeEdramSetSize(0x400000);
#endif

#if BUILD_PLAT != BUILD_VITA
    SC_PROFILE_BEGIN_SESSION("Init", "SC-Init.json");
    SC_PROFILE_FUNCTION(init_sc, __LINE__, __FILE__);

    auto app = CreateNewSCApp();
    app->on_start();

    SC_PROFILE_END_SESSION();

    SC_PROFILE_BEGIN_SESSION("User Session", "SC-User.json");
    app->run();
    SC_PROFILE_END_SESSION();

    SC_PROFILE_BEGIN_SESSION("Cleanup", "SC-Cleanup.json");

    delete app;

    SC_PROFILE_FUNCTION(cleanup_sc, __LINE__, __FILE__)
    SC_PROFILE_END_SESSION();

#elif BUILD_PLAT == BUILD_VITA
    sceSysmoduleLoadModule(SCE_SYSMODULE_RAZOR_CAPTURE);

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceIoMkdir("ux0:/data/SDC", 0777);
    SC_PROFILE_BEGIN_SESSION("Init", "ux0:/data/SDC/SC-Init.json");
    SC_PROFILE_FUNCTION(init_sc, __LINE__, __FILE__);

    auto app = CreateNewSCApp();
    app->on_start();

    SC_PROFILE_END_SESSION();

    SC_PROFILE_BEGIN_SESSION("User Session", "SC-User.json");
    app->run();
    SC_PROFILE_END_SESSION();

    SC_PROFILE_BEGIN_SESSION("Cleanup", "SC-Cleanup.json");

    delete app;

    SC_PROFILE_FUNCTION(cleanup_sc, __LINE__, __FILE__)
    SC_PROFILE_END_SESSION();

    sceKernelExitProcess(EXIT_SUCCESS);
#endif

    return EXIT_SUCCESS;
}