#include <Network/Socket.hpp>

#if BUILD_PLAT == BUILD_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <Platform/Platform.hpp>

namespace Stardust_Celeste::Network {
ServerSocket::ServerSocket(u16 port_number) {
  my_socket = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));

  if (my_socket == -1) {
    throw std::runtime_error("Fatal: Could not open socket! Errno: " +
                             std::to_string(errno));
  }
  port = port_number;

  SC_CORE_DEBUG("Socket Created!");

  sockaddr_in sockaddr{};
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(port);

  if (bind(my_socket, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    throw std::runtime_error(
        "Fatal: Could not bind socket address! Port: " + std::to_string(port) +
        ". Errno: " + std::to_string(errno));
  }

  SC_CORE_DEBUG("Socket Bound!");
}

ServerSocket::~ServerSocket() { Close(); }

void ServerSocket::Close() const {
  SC_CORE_INFO("Closing Server Socket!");

#if BUILD_PLAT == BUILD_WINDOWS
  closesocket(my_socket);
#else
  close(my_socket);
#endif
}

auto ServerSocket::ListenState() const -> RefPtr<Connection> {
  sockaddr_in socketAddress{};
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_addr.s_addr = INADDR_ANY;
  socketAddress.sin_port = htons(port);

  SC_CORE_DEBUG("Listening on socket...");
  if (listen(my_socket, 1) < 0) {
    throw std::runtime_error("Fatal: Could not listen on socket. Errno: " +
                             std::to_string(errno));
  }

  auto addressLen = sizeof(socketAddress);
  auto conn = static_cast<int>(accept(
      my_socket, (struct sockaddr *)&socketAddress, (socklen_t *)&addressLen));
  SC_CORE_DEBUG("Found potential connection...");

  if (conn < 0) {
    throw std::runtime_error("Fatal: Could not accept connection. Errno: " +
                             std::to_string(errno));
  }

  SC_CORE_INFO("New Connection from " +
               std::string((inet_ntoa(socketAddress.sin_addr))) + " on port " +
               std::to_string(ntohs(socketAddress.sin_port)));

  return createRefPtr<Connection>(conn);
}
} // namespace Stardust_Celeste::Network