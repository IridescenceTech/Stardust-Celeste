#include <Network/NetworkDriver.hpp>

#if BUILD_PLAT == BUILD_WINDOWS
#include <winsock2.h>
#endif

#if BUILD_PLAT == BUILD_PSP
#include <pspkernel.h>
#include <pspmodulemgr.h>
#include <pspnet.h>
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
  Utilities::detail::core_Logger->log("Attempting Network Init");
  sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
  sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
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
  sceUtilityUnloadNetModule(PSP_NET_MODULE_PARSEURI);
  sceUtilityUnloadNetModule(PSP_NET_MODULE_PARSEHTTP);
  sceUtilityUnloadNetModule(PSP_NET_MODULE_HTTP);

  SC_CORE_INFO("Cleaning up Networking Driver");
#endif
}
} // namespace Stardust_Celeste::Network