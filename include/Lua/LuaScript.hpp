#pragma once
#include <string>
#include <Utilities/Singleton.hpp>

namespace Stardust_Celeste::Scripting {

    /**
     * Lua Script Object contains a script object within the current Lua Context
     */
    class LuaScript {
    public:
        /**
         * Loads a script from a given filename.
         * @param filename Lua File to load.
         */
        LuaScript(std::string filename);

        /**
         * Lua Script Destructor
         */
        ~LuaScript();

        /**
         * Calls a function
         * @param functionName Name of the function to call
         */
        auto callFunction(std::string functionName) -> void;

        /**
         * Runs the script from the top
         */
        auto run() -> void;

        /**
         * Calls the onUpdate() function
         */
        auto update() -> void;

        /**
         * Calls the onDraw() function
         */
        auto draw() -> void;
    };
}