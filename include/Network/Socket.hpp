/**
 * @file Socket.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Implements a socket base class
 * @version 0.1
 * @date 2021-12-20
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#include <Utilities/Types.hpp>

#if BUILD_PLAT == BUILD_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <Network/Packet.hpp>

namespace Stardust_Celeste::Network {

class Socket {
  public:
    virtual ~Socket();

    auto send(ScopePtr<PacketOut>) const -> void;
    auto recv() const -> RefPtr<PacketIn>;

    auto set_blocking(bool blocking) -> bool;

    auto close() const -> void;
    auto is_alive() const -> bool;

  protected:
    s32 my_socket = 0;
};

class ClientSocket : public Socket {
  public:
    ClientSocket();
    ~ClientSocket() override = default;

    /**
     * Connects to an IP at a port number.
     **/
    auto connect(unsigned short port, const char *ip) -> bool;
};

class Connection : public Socket {
  public:
    Connection(s32 sock);
};

class ServerSocket {
  public:
    ServerSocket(u16 port_number);
    ~ServerSocket();

    auto listen() const -> RefPtr<Connection>;
    auto close() const -> void;

  private:
    s32 my_socket;
    u16 port;
};
} // namespace Stardust_Celeste::Network