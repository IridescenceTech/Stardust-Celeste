#include <Network/NetworkDriver.hpp>

#if BUILD_PLAT == BUILD_WINDOWS
#include <winsock2.h>
#endif

namespace Stardust_Celeste::Network {
auto NetworkDriver::init() -> bool {
#if BUILD_PLAT == BUILD_WINDOWS
  WSAData data;
  int res = WSAStartup(MAKEWORD(2, 2), &data);
  if (res != 0) {
    return false;
  }
#endif
  return true;
}

auto NetworkDriver::cleanup() -> void {
#if BUILD_PLAT == BUILD_WINDOWS
  WSACleanup();
#endif
}
} // namespace Stardust_Celeste::Network