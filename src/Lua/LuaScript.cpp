#include <Lua/LuaScript.hpp>
#include <Lua/LuaContext.hpp>
#include <Utilities/Logger.hpp>
#include <filesystem>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace Stardust_Celeste::Scripting {

    LuaScript::LuaScript(std::string filename) {
        auto L = reinterpret_cast<lua_State*>(LuaContext::get().lua_context);
        if(auto err = luaL_loadfile(L, filename.c_str())) {
            SC_CORE_ERROR("Failed to load Lua script {}; code {}\n", filename, err);

            auto error = lua_tostring(L, -1);
            std::cerr << error << std::endl;

            throw std::runtime_error("Failed to load Lua script " + filename);
        }
    }

    LuaScript::~LuaScript() {

    }

    auto LuaScript::callFunction(std::string functionName) -> void {
        auto L = reinterpret_cast<lua_State*>(LuaContext::get().lua_context);

        lua_getglobal(L, functionName.c_str());

        if (lua_pcall(L, 0, 0, 0) != 0) {
            auto err = lua_tostring(L, -1);
            SC_CORE_ERROR("Running script function {} failed: {}", functionName, err);
        }
    }

    auto LuaScript::run() -> void {
        auto L = reinterpret_cast<lua_State*>(LuaContext::get().lua_context);

        if (lua_pcall(L, 0, 0, 0) != 0) {
            auto err = lua_tostring(L, -1);
            SC_CORE_ERROR("Running script failed: {}", err);
        }
    }

    auto LuaScript::update() -> void {
        callFunction("onUpdate");
    }

    auto LuaScript::draw() -> void {
        callFunction("onDraw");
    }

}