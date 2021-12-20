#include <Network/Socket.hpp>

#if BUILD_PLAT == BUILD_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

namespace Stardust_Celeste::Network {
ClientSocket::ClientSocket() { my_socket = 0; }

auto ClientSocket::Connect(unsigned short port, const char *ip) -> bool {
  SC_CORE_INFO("Opening Connection: [" + std::string(ip) + "]" + "@" +
               std::to_string(port));

  my_socket = static_cast<int>(socket(PF_INET, SOCK_STREAM, 0));
  struct sockaddr_in name {};
  name.sin_family = AF_INET;
  name.sin_port = htons(port);

  inet_pton(AF_INET, ip, &name.sin_addr.s_addr);
  bool b = (connect(my_socket, (struct sockaddr *)&name, sizeof(name)) >= 0);

  if (!b) {
    SC_CORE_ERROR("Failed to open a connection! (Is the server open?)");
  }

  return b;
}
} // namespace Stardust_Celeste::Network