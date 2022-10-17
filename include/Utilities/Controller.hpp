#pragma once
#include "Types.hpp"
#include <Platform/Platform.hpp>
#include <vector>

namespace Stardust_Celeste::Utilities {

enum KeyFlag {
    None = 0x00,
    Press = 0x01,
    Held = 0x02,
    Release = 0x04,
    Untouched = 0x08
};

struct KeyData {
    int key;
    int flags;
};

struct KeyCommandPair {
    KeyData key;
    Command cmd;
};

class Controller {
  public:
    Controller() = default;
    virtual ~Controller() = default;

    /**
     * @brief Adds a command to the controller
     *
     * @param key "Key" to be bound
     * @param command Command to be executed
     */
    inline auto add_command(KeyData key, Command command) -> void {
        command_map.push_back({key, command});
    }

    /**
     * @brief Clears all stored commands
     *
     */
    inline auto clear_command() -> void { command_map.clear(); }

    /**
     * @brief Updates the controller and executes given commands
     *
     */
    virtual void update() = 0;

  protected:
    std::vector<KeyCommandPair> command_map;
};
} // namespace Stardust_Celeste::Utilities