#include <Network/Socket.hpp>

namespace Stardust_Celeste::Network {
Connection::Connection(s32 sock) {
  my_socket = sock;
  SetBlocking(false);
}
} // namespace Stardust_Celeste::Network