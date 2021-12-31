#include <Network/Socket.hpp>

namespace Stardust_Celeste::Network {
Connection::Connection(s32 sock) {
    my_socket = sock;
    set_blocking(false);
}
} // namespace Stardust_Celeste::Network