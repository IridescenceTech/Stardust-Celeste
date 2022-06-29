#include <Network/NetworkDriver.hpp>
#include <Utilities/Logger.hpp>

#if BUILD_PLAT == BUILD_WINDOWS
#include <winsock2.h>
#elif BUILD_PLAT == BUILD_PSP
#include <malloc.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <pspmodulemgr.h>
#include <pspnet.h>
#include <pspnet_apctl.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>
#include <psppower.h>
#include <psptypes.h>
#include <psputility.h>
#include <psputils.h>
#include <stdarg.h>
#include <string.h>
#endif

namespace Stardust_Celeste::Network {
auto NetworkDriver::init() -> bool {
#if BUILD_PLAT == BUILD_WINDOWS
    WSAData data;
    int res = WSAStartup(MAKEWORD(2, 2), &data);
    if (res != 0) {
        return false;
    }
#elif BUILD_PLAT == BUILD_PSP
    SC_CORE_INFO("Attempting Network Init");
    sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
    sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
    sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEURI);
    sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEHTTP);
    sceUtilityLoadNetModule(PSP_NET_MODULE_HTTP);

    // Result stores our codes from the initialization process
    int result = 0;
    result = sceNetInit(128 * 1024, 42, 0, 42,
                        0); // Creates the network manager with a buffer
    if (result < 0) {       // These If Blocks close the game on an error

        SC_CORE_WARN("Failed sceNetInit");
        return false;
    }
    result = sceNetInetInit(); // Initializes Inet
    if (result < 0) {
        SC_CORE_WARN("Failed sceInetInit");
        return false;
    }
    result = sceNetApctlInit(0x10000, 48); // Initializes Access Point Control
    if (result < 0) {
        SC_CORE_WARN("Failed sceNetApctlInit");
        return false;
    }
    result = sceNetResolverInit(); // Initializes DNS resolver (unused)
    if (result < 0) {
        SC_CORE_WARN("Failed sceNetResolverInit");
        return false;
    }

    int err;
    int stateLast = -1;

    /* Connect using the first profile */
    err = sceNetApctlConnect(1);
    if (err != 0) {
        return 0;
    }

    while (1) {
        int state;
        err = sceNetApctlGetState(&state);
        if (err != 0) {
            break;
        }
        if (state > stateLast) {
            stateLast = state;
        }
        if (state == 4)
            break; // connected with static IP

        // wait a little before polling again
        sceKernelDelayThread(50 * 1000); // 50ms
    }
    if (err != 0) {
        return 0;
    }

    return 1;
#endif
    return true;
}

#if BUILD_PLAT == BUILD_PSP
char list[0x10] __attribute__((aligned(64)));
auto ShowNetworkDialog() -> bool {
    pspUtilityNetconfData networkData;
    bool done = true;
    int result = -1;

    memset(&networkData, 0, sizeof(pspUtilityNetconfData));
    networkData.base.size = sizeof(networkData);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,
                                &networkData.base.language);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN,
                                &networkData.base.buttonSwap);
    networkData.base.graphicsThread = 17;
    networkData.base.accessThread = 19;
    networkData.base.fontThread = 18;
    networkData.base.soundThread = 16;
    networkData.action = PSP_NETCONF_ACTION_CONNECTAP;

    struct pspUtilityNetconfAdhoc adhocparam;
    memset(&adhocparam, 0, sizeof(adhocparam));
    networkData.adhocparam = &adhocparam;

    sceUtilityNetconfInitStart(&networkData);

    while (done) {
        sceGuStart(GU_DIRECT, list);
        sceGuClear(GU_COLOR_BUFFER_BIT);
        sceGuFinish();
        sceGuSync(0, 0);

        switch (sceUtilityNetconfGetStatus()) {
        case PSP_UTILITY_DIALOG_NONE: {
            result = networkData.base.result;
            done = false;
        } break;

        case PSP_UTILITY_DIALOG_VISIBLE:
            sceUtilityNetconfUpdate(1);
            break;

        case PSP_UTILITY_DIALOG_QUIT:
            sceUtilityNetconfShutdownStart();
            break;

        case PSP_UTILITY_DIALOG_FINISHED:
            break;

        default:
            break;
        }

        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    if (result == 0)
        return true;

    return false;
}

auto NetworkDriver::initGUI() -> bool {
    SC_CORE_INFO("Attempting Network Init");
    sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
    sceUtilityLoadNetModule(PSP_NET_MODULE_INET);

    // Result stores our codes from the initialization process
    int result = 0;
    result = sceNetInit(128 * 1024, 42, 0, 42,
                        0); // Creates the network manager with a buffer
    if (result < 0) {       // These If Blocks close the game on an error

        SC_CORE_WARN("Failed sceNetInit");
        return false;
    }
    result = sceNetInetInit(); // Initializes Inet
    if (result < 0) {
        SC_CORE_WARN("Failed sceInetInit");
        return false;
    }
    result = sceNetApctlInit(0x10000, 48); // Initializes Access Point Control
    if (result < 0) {
        SC_CORE_WARN("Failed sceNetApctlInit");
        return false;
    }

    return ShowNetworkDialog();
}
#endif

auto NetworkDriver::cleanup() -> void {
#if BUILD_PLAT == BUILD_WINDOWS
    WSACleanup();
#elif BUILD_PLAT == BUILD_PSP
    // NET
    sceNetApctlTerm();
    sceNetResolverTerm();
    sceNetInetTerm();
    sceNetTerm();

    // Modules
    sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);
    sceUtilityUnloadNetModule(PSP_NET_MODULE_INET);

    SC_CORE_INFO("Cleaning up Networking Driver");
#endif
}
} // namespace Stardust_Celeste::Network