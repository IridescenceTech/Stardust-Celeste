#include <Lua/LuaContext.hpp>
#include <Rendering/RenderContext.hpp>
#include <Rendering/Texture.hpp>
#include <Utilities/Utilities.hpp>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

void registerLibrary(const char* libname, const luaL_Reg* library) {
    auto L = reinterpret_cast<lua_State*>(Stardust_Celeste::Scripting::LuaContext::get().lua_context);
    lua_getglobal(L, libname);

    if(lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }

    luaL_setfuncs(L, library, 0);
    lua_setglobal(L, libname);
}

#define _L lua_State* L
#define SDC_LAPI static int

namespace Modules::Utilities {

    SDC_LAPI _logtrace(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: Logger.trace() takes 1 argument.");
        }

        auto msg = luaL_checkstring(L, 1);
        SC_APP_TRACE("{}", msg);

        return 0;
    }

    SDC_LAPI _logdebug(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: Logger.trace() takes 1 argument.");
        }

        auto msg = luaL_checkstring(L, 1);
        SC_APP_DEBUG("{}", msg);

        return 0;
    }

    SDC_LAPI _loginfo(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: Logger.trace() takes 1 argument.");
        }

        auto msg = luaL_checkstring(L, 1);
        SC_APP_INFO("{}", msg);

        return 0;
    }

    SDC_LAPI _logwarn(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: Logger.trace() takes 1 argument.");
        }

        auto msg = luaL_checkstring(L, 1);
        SC_APP_WARN("{}", msg);

        return 0;
    }

    SDC_LAPI _logerror(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: Logger.trace() takes 1 argument.");
        }

        auto msg = luaL_checkstring(L, 1);
        SC_APP_ERROR("{}", msg);

        return 0;
    }

    static const luaL_Reg logLib [] = {
            {"trace", _logtrace},
            {"debug", _logdebug},
            {"info", _loginfo},
            {"warn", _logwarn},
            {"error", _logerror},
            {0, 0}
    };

    void initialize_utils() {
        registerLibrary("Logger", logLib);
    }
}

namespace Modules::Rendering {
    using namespace Stardust_Celeste;

    SDC_LAPI _rctxclear(_L) {
        int argc = lua_gettop(L);
        if (argc != 0) {
            return luaL_error(L, "Error: RenderContext.clear() takes 0 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->clear();

        return 0;
    }

    SDC_LAPI _rctxclearcolor(_L) {
        int argc = lua_gettop(L);
        if (argc != 4) {
            return luaL_error(L, "Error: RenderContext.clearColor() takes 4 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());

        u8 r = luaL_checkinteger(L, 1);
        u8 g = luaL_checkinteger(L, 2);
        u8 b = luaL_checkinteger(L, 3);
        u8 a = luaL_checkinteger(L, 4);

        rctx->set_color({r, g, b, a});

        return 0;
    }


    SDC_LAPI _rctxmatrixpush(_L) {
        int argc = lua_gettop(L);
        if(argc != 0) {
            return luaL_error(L, "Error: RenderContext.matrixPush() takes 0 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_push();

        return 0;

    }

    SDC_LAPI _rctxmatrixpop(_L) {
        int argc = lua_gettop(L);
        if(argc != 0) {
            return luaL_error(L, "Error: RenderContext.matrixPop() takes 0 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_pop();

        return 0;

    }

    SDC_LAPI _rctxmatrixclear(_L) {
        int argc = lua_gettop(L);
        if(argc != 0) {
            return luaL_error(L, "Error: RenderContext.matrixClear() takes 0 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_clear();

        return 0;
    }

    SDC_LAPI _rctxmatrixset2D(_L) {
        int argc = lua_gettop(L);
        if(argc != 0) {
            return luaL_error(L, "Error: RenderContext.set2D() takes 0 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->set_mode_2D();

        return 0;
    }

    SDC_LAPI _rctxmatrixset3D(_L) {
        int argc = lua_gettop(L);
        if(argc != 0) {
            return luaL_error(L, "Error: RenderContext.set3D() takes 0 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->set_mode_3D();

        return 0;
    }


    SDC_LAPI _rctxmatrixtranslate(_L) {
        int argc = lua_gettop(L);
        if(argc != 3) {
            return luaL_error(L, "Error: RenderContext.matrixTranslate() takes 3 arguments.");
        }

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_translate({static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)});

        return 0;
    }

    SDC_LAPI _rctxmatrixrotate(_L) {
        int argc = lua_gettop(L);
        if(argc != 3) {
            return luaL_error(L, "Error: RenderContext.matrixRotate() takes 3 arguments.");
        }

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_rotate({static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)});

        return 0;
    }

    SDC_LAPI _rctxmatrixscale(_L) {
        int argc = lua_gettop(L);
        if(argc != 3) {
            return luaL_error(L, "Error: RenderContext.matrixScale() takes 3 arguments.");
        }

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_scale({static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)});

        return 0;
    }

    SDC_LAPI _rctxmatrixperspective(_L) {
        int argc = lua_gettop(L);
        if(argc != 4) {
            return luaL_error(L, "Error: RenderContext.matrixPerspective() takes 4 arguments.");
        }

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);
        auto w = luaL_checknumber(L, 4);

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_perspective(x, y, z, w);

        return 0;
    }

    SDC_LAPI _rctxmatrixortho(_L) {
        int argc = lua_gettop(L);
        if(argc != 6) {
            return luaL_error(L, "Error: RenderContext.matrixOrtho() takes 4 arguments.");
        }

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);
        auto w = luaL_checknumber(L, 4);
        auto u = luaL_checknumber(L, 5);
        auto v = luaL_checknumber(L, 6);

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->matrix_ortho(x, y, z, w, u, v);

        return 0;
    }
    SDC_LAPI _rctxsetvsync(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: RenderContext.setVsync() takes 1 argument.");
        }

        auto x = luaL_checkinteger(L, 1);
        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());
        rctx->vsync = x;

        return 0;
    }

    static const luaL_Reg rctxLib [] = {
            {"clear", _rctxclear},
            {"clearColor", _rctxclearcolor},
            {"matrixPush", _rctxmatrixpush},
            {"matrixPop", _rctxmatrixpop},
            {"matrixClear", _rctxmatrixclear},
            {"set2D", _rctxmatrixset2D},
            {"set3D", _rctxmatrixset3D},
            {"matrixTranslate", _rctxmatrixtranslate},
            {"matrixRotate", _rctxmatrixrotate},
            {"matrixScale", _rctxmatrixscale},
            {"matrixPerspective", _rctxmatrixperspective},
            {"matrixOrtho", _rctxmatrixortho},
            {"setVsync", _rctxsetvsync},
            {0, 0}
    };

    void initialize_rendering() {
        registerLibrary("RenderContext", rctxLib);
    }

    SDC_LAPI _texmbind(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: TextureManager.bind() takes 1 argument.");
        }

        auto x = luaL_checkinteger(L, 1);

        Stardust_Celeste::Rendering::TextureManager::get().bind_texture(x);
        return 0;
    }

    SDC_LAPI _texmload(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: TextureManager.load() takes 1 argument.");
        }

        auto file = luaL_checkstring(L, 1);

        auto x = Stardust_Celeste::Rendering::TextureManager::get().load_texture(file, SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST, true);

        lua_pushinteger(L, x);
        return 1;
    }

    SDC_LAPI _texmdelete(_L) {
        int argc = lua_gettop(L);
        if(argc != 1) {
            return luaL_error(L, "Error: TextureManager.delete() takes 1 argument.");
        }

        auto x = luaL_checkinteger(L, 1);

        Stardust_Celeste::Rendering::TextureManager::get().delete_texture(x);
        return 0;
    }


    static const luaL_Reg texLib [] = {
            {"load", _texmload},
            {"bind", _texmbind},
            {"delete", _texmdelete},
            {0, 0}
    };

    void initialize_texture() {
        registerLibrary("TextureManager", texLib);
    }
}

namespace Stardust_Celeste::Scripting {
    LuaContext::~LuaContext() {
        cleanup();
    }

    auto LuaContext::init() -> void {
        lua_context = luaL_newstate();
        luaL_openlibs(reinterpret_cast<lua_State*>(lua_context));

        // Load Modules
        Modules::Utilities::initialize_utils();

        Modules::Rendering::initialize_rendering();
        Modules::Rendering::initialize_texture();
    }

    auto LuaContext::cleanup() -> void {
        lua_close(reinterpret_cast<lua_State*>(lua_context));
    }
}