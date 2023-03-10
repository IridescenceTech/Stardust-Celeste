#include <Lua/LuaContext.hpp>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace Stardust_Celeste::Scripting {
    LuaContext::~LuaContext() {
        cleanup();
    }

    auto LuaContext::init() -> void {
        lua_context = luaL_newstate();
        luaL_openlibs(reinterpret_cast<lua_State*>(lua_context));
    }

    auto LuaContext::cleanup() -> void {
        lua_close(reinterpret_cast<lua_State*>(lua_context));
    }
}