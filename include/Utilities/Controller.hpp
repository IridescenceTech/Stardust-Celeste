#pragma once
#include "Types.hpp"
#include <Platform/Platform.hpp>
#include <map>

namespace Stardust_Celeste::Utilities {
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
    inline auto add_command(int key, Command command) -> void {
        command_map.emplace(key, command);
    }

    /**
     * @brief Removes a single command from the controller
     *
     * @param key Key to the command to be removed
     */
    inline auto remove_command(int key) -> void { command_map.erase(key); }

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
    std::map<int, Command> command_map;
};
} // namespace Stardust_Celeste::Utilities