#pragma once
#include <string>
#include <Utilities/Singleton.hpp>

namespace Stardust_Celeste::Scripting {

    class LuaScript {
    public:
        LuaScript(std::string filename);
        ~LuaScript();

        auto callFunction(std::string functionName) -> void;

        auto run() -> void;

        auto update() -> void;
        auto draw() -> void;
    };
}