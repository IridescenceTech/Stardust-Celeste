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
#include <vector>

namespace Stardust_Celeste::Network {
auto Socket::close() const -> void {
    SC_CORE_INFO("Closing Client Socket!");

#if BUILD_PLAT == BUILD_WINDOWS
    closesocket(my_socket);
#else
    ::close(my_socket);
#endif
}
Socket::~Socket() { close(); }

auto Socket::send(ScopePtr<PacketOut> packetOut) const -> void {
    size_t packetLength = packetOut->buffer->GetUsedSpace() + 1;

    auto byteBuffer = create_scopeptr<ByteBuffer>(packetLength + 5); // 512 KB
    byteBuffer->WriteVarI32(static_cast<uint32_t>(packetLength));
    byteBuffer->WriteI8(packetOut->ID);

    for (int i = 0; i < packetOut->buffer->GetUsedSpace(); i++) {
        uint8_t temp;
        packetOut->buffer->ReadU8(temp);
        byteBuffer->WriteU8(temp);
    }

    int res = ::send(my_socket, byteBuffer->m_Buffer,
                     static_cast<int>(byteBuffer->GetUsedSpace()), 0);
    if (res < 0) {
        SC_CORE_WARN("Failed to send a packet!");
        SC_CORE_WARN("Packet Size: " +
                     std::to_string(packetOut->buffer->GetUsedSpace()));
    }
}

auto Socket::is_alive() const -> bool {
    bool connected = false;
    char buffer[32] = {0};
    int res = ::recv(my_socket, buffer, sizeof(buffer), MSG_PEEK);

    if (res != 0) {
        connected = true;
    }

    return connected;
}

auto Socket::recv(int size) const -> RefPtr<PacketIn> {

    if (size == -1) {
        std::vector<u8> len;
        u8 newByte;
        int res =
            ::recv(my_socket, reinterpret_cast<char *>(&newByte), 1, MSG_PEEK);

        if (res > 0) {
            char data[5] = {0};
            size_t dataLen = 0;
            do {
                size_t totalReceived = 0;
                while (1 > totalReceived) {
                    size_t received =
                        ::recv(my_socket, &data[dataLen] + totalReceived,
                               static_cast<int>(1 - totalReceived), 0);
                    if (received <= 0) {
                        delay(1);
                    } else {
                        totalReceived += received;
                    }
                }
            } while ((data[dataLen++] & 0x80) != 0);

            int readed = 0;
            int result = 0;
            char read;
            do {
                read = data[readed];
                int value = (read & 0b01111111);
                result |= (value << (7 * readed));
                readed++;
            } while ((read & 0b10000000) != 0);

            int length = result;

            auto pIn = new PacketIn(length);
            SC_CORE_DEBUG("LENGTH: " + std::to_string(length));

            int totalTaken = 0;

            u8 *b = new u8[length];
            for (int i = 0; i < length; i++) {
                b[i] = 0;
            }

            while (totalTaken < length) {
                res = ::recv(my_socket, reinterpret_cast<char *>(b), length, 0);
                if (res > 0) {
                    totalTaken += res;
                } else {
                    delay(1);
                }
            }

            for (int i = 0; i < length; i++) {
                pIn->buffer->WriteU8(b[i]);
            }

            delete[] b;

            if (pIn->buffer->GetUsedSpace() > 0) {
                uint8_t t = 0;
                pIn->buffer->ReadU8(t);
                pIn->ID = t;
            } else {
                pIn->ID = -1;
            }

            SC_CORE_DEBUG("Received Packet!");
            SC_CORE_DEBUG("Packet ID: " + std::to_string(pIn->ID));

            return RefPtr<PacketIn>{pIn};
        } else {
            return nullptr;
        }
    } else {
        u8 *b = new u8[size];

        size_t totalReceived = 0;
        while (size > totalReceived) {
            size_t received = ::recv(my_socket, b + totalReceived,
                                     static_cast<int>(1 - totalReceived), 0);
            if (received <= 0) {
                delay(1);
            } else {
                totalReceived += received;
            }
        }
        auto pIn = new PacketIn(size);

        for (int i = 0; i < size; i++) {
            pIn->buffer->WriteU8(b[i]);
        }

        delete[] b;
        return RefPtr<PacketIn>{pIn};
    }
}

auto Socket::set_blocking(bool blocking) -> bool {

#if BUILD_PLAT == BUILD_WINDOWS
    unsigned long iMode = !blocking;
    int iResult = ioctlsocket(my_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR) {
        return false;
    }
    return true;
#else
    if (blocking) {
        int flags = fcntl(my_socket, F_GETFL, 0);
        flags &= ~O_NONBLOCK;
        fcntl(my_socket, F_SETFL, flags);
    } else {
        fcntl(my_socket, F_SETFL, O_NONBLOCK);
    }
    return true;
#endif
}
} // namespace Stardust_Celeste::Network