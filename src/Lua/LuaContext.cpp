#include <Lua/LuaContext.hpp>
#include <Rendering/RenderContext.hpp>
#include <Rendering/Texture.hpp>
#include <Utilities/Utilities.hpp>
#include <Audio/Clip.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Graphics/2D/AnimatedSprite.hpp>
#include <Math/MathUtils.hpp>
#include <Rendering/Camera.hpp>
#include <Graphics/2D/AnimatedTilemap.hpp>
#include <Graphics/2D/Tilemap.hpp>

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

    SDC_LAPI _timercreate(_L) {
        int argc = lua_gettop(L);
        if (argc != 0)
            return luaL_error(L, "Error: Timer.create() takes 0 arguments.");

        void* ptimer = lua_newuserdata(L,sizeof(Stardust_Celeste::Utilities::Timer));
        auto timer = static_cast<Stardust_Celeste::Utilities::Timer*>(ptimer);
        timer = new Stardust_Celeste::Utilities::Timer();
        timer->reset();

        luaL_getmetatable(L, "Timer");
        lua_setmetatable(L, -2);

        return 1;
    }

    Stardust_Celeste::Utilities::Timer* getTimer(lua_State* L){
        return (Stardust_Celeste::Utilities::Timer*)luaL_checkudata(L, 1, "Timer");
    }

    SDC_LAPI _timerdestroy(_L) {
        auto t = getTimer(L);
        delete t;

        return 0;
    }

    static int _timerdelta(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Timer.delta() takes 1 argument.");

        auto t = getTimer(L);
        float delta = t->get_delta_time();

        lua_pushnumber(L, delta);
        return 1;
    }

    static int _timerreset(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Timer.reset() takes 1 argument.");

        auto t = getTimer(L);
        t->reset();

        return 0;
    }

    static int _timerelapsed(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Timer.elapsed() takes 1 argument.");

        auto t = getTimer(L);
        float elapsed = t->elapsed();

        lua_pushnumber(L, elapsed);
        return 1;
    }

    static const luaL_Reg timerLib [] = { // Timer methods
            {"create", _timercreate},
            {"destroy", _timerdestroy},
            {"delta", _timerdelta},
            {"reset", _timerreset},
            {"elapsed", _timerelapsed},
            {0,0}
    };

    static const luaL_Reg timerMetaLib [] = {
            {"__gc", _timerdestroy},
            {0,0}
    };

    SDC_LAPI _todeg(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: MathUtil.toDeg() takes 1 argument.");

        auto x = luaL_checknumber(L, 1);

        auto r = Stardust_Celeste::Math::toDegrees(x);

        lua_pushnumber(L, r);
        return 1;
    }

    SDC_LAPI _torad(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: MathUtil.toRad() takes 1 argument.");

        auto x = luaL_checknumber(L, 1);

        auto r = Stardust_Celeste::Math::toRadians(x);

        lua_pushnumber(L, r);
        return 1;
    }

    SDC_LAPI _sign(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: MathUtil.sign() takes 1 argument.");

        auto x = luaL_checknumber(L, 1);

        auto r = Stardust_Celeste::Math::sign(x);

        lua_pushnumber(L, r);
        return 1;
    }

    SDC_LAPI _clamp(_L) {
        int argc = lua_gettop(L);
        if (argc != 3)
            return luaL_error(L, "Error: MathUtil.clamp() takes 3 arguments.");

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);

        auto r = Stardust_Celeste::Math::clamp(x, y, z);

        lua_pushnumber(L, r);
        return 1;
    }

    SDC_LAPI _lerp(_L) {
        int argc = lua_gettop(L);
        if (argc != 3)
            return luaL_error(L, "Error: MathUtil.lerp() takes 3 arguments.");

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);

        auto r = Stardust_Celeste::Math::lerp(x, y, z);

        lua_pushnumber(L, r);
        return 1;
    }

    SDC_LAPI _smoothstep(_L) {
        int argc = lua_gettop(L);
        if (argc != 3)
            return luaL_error(L, "Error: MathUtil.smoothstep() takes 3 arguments.");

        auto x = luaL_checknumber(L, 1);
        auto y = luaL_checknumber(L, 2);
        auto z = luaL_checknumber(L, 3);

        auto r = Stardust_Celeste::Math::smoothstep(x, y, z);

        lua_pushnumber(L, r);
        return 1;
    }

    static const luaL_Reg mathuLib [] = {
            {"toDeg", _todeg},
            {"toRad", _torad},
            {"sign", _sign},
            {"clamp", _clamp},
            {"lerp", _lerp},
            {"smoothstep", _smoothstep},
            {0, 0}
    };

    void initialize_utils() {
        registerLibrary("Logger", logLib);
        registerLibrary("MathUtil", mathuLib);

        auto L = reinterpret_cast<lua_State*>(Stardust_Celeste::Scripting::LuaContext::get().lua_context);

        int lib_id, meta_id;

        // new class = {}
        lua_createtable(L, 0, 0);
        lib_id = lua_gettop(L);

        // meta table = {}
        luaL_newmetatable(L, "Timer");
        meta_id = lua_gettop(L);
        luaL_setfuncs(L, timerMetaLib, 0);

        // meta table = methods
        luaL_newlib(L, timerLib);
        lua_setfield(L, meta_id, "__index");

        // meta table.metatable = metatable
        luaL_newlib(L, timerMetaLib);
        lua_setfield(L, meta_id, "__metatable");

        // class.metatable = metatable
        lua_setmetatable(L, lib_id);

        lua_setglobal(L, "Timer");
    }
}

namespace Modules::Audio {

    SDC_LAPI _audioload(_L) {
        int argc = lua_gettop(L);
        if (argc != 2)
            return luaL_error(L, "Error: AudioClip.load() takes 2 arguments.");


        const char* filename = luaL_checkstring(L, 1);
        bool streaming = luaL_checkinteger(L, 2);

        void* pptr_location = lua_newuserdata(L, sizeof(Stardust_Celeste::Audio::Clip*));
        Stardust_Celeste::Audio::Clip** pptr = static_cast<Stardust_Celeste::Audio::Clip **>(pptr_location);
        *pptr = new Stardust_Celeste::Audio::Clip(filename, streaming);

        luaL_getmetatable(L, "AudioClip");
        lua_setmetatable(L, -2);

        return 1;
    }

    Stardust_Celeste::Audio::Clip** getClip(lua_State* L){
        return (Stardust_Celeste::Audio::Clip**)luaL_checkudata(L, 1, "AudioClip");
    }

    SDC_LAPI _audiodestroy(_L) {
        auto clip = getClip(L);
        delete (*clip);

        return 0;
    }

    SDC_LAPI _audioloop(_L) {
        auto clip = *getClip(L);

        int argc = lua_gettop(L);

        if(argc != 2) {
            return luaL_error(L, "Error: AudioClip.set_loop() takes 2 argument.");
        }

        bool b = luaL_checkinteger(L, 2);
        clip->set_looping(b);

        return 0;
    }

    SDC_LAPI _audiovolume(_L) {
        auto clip = *getClip(L);

        int argc = lua_gettop(L);

        if(argc != 2) {
            return luaL_error(L, "Error: AudioClip.set_volume() takes 2 arguments.");
        }

        float b = luaL_checknumber(L, 2);
        clip->set_volume(b);

        return 0;
    }

    SDC_LAPI _audioplay(_L) {
        auto clip = *getClip(L);

        int argc = lua_gettop(L);

        if(argc != 1) {
            return luaL_error(L, "Error: AudioClip.play() takes 1 argument.");
        }

        clip->play();

        return 0;
    }

    SDC_LAPI _audiopause(_L) {
        auto clip = *getClip(L);

        int argc = lua_gettop(L);

        if(argc != 1) {
            return luaL_error(L, "Error: AudioClip.pause() takes 1 argument.");
        }

        clip->pause();

        return 0;
    }

    SDC_LAPI _audiostop(_L) {
        auto clip = *getClip(L);

        int argc = lua_gettop(L);

        if(argc != 0) {
            return luaL_error(L, "Error: AudioClip.stop() takes 0 arguments.");
        }

        clip->stop();

        return 0;
    }

    static const luaL_Reg audioLib[] = {
            {"load", _audioload},
            {"destroy", _audiodestroy},
            {"set_loop", _audioloop},
            {"set_volume", _audiovolume},
            {"play", _audioplay},
            {"pause", _audiopause},
            {"stop", _audiostop},
            {0,0}
    };

    static const luaL_Reg audioMetaLib[] = {
            {"__gc", _audiodestroy},
            {0,0}
    };

    void initialize_audio() {
        auto L = reinterpret_cast<lua_State*>(Stardust_Celeste::Scripting::LuaContext::get().lua_context);

        int lib_id, meta_id;

        // new class = {}
        lua_createtable(L, 0, 0);
        lib_id = lua_gettop(L);

        // meta table = {}
        luaL_newmetatable(L, "AudioClip");
        meta_id = lua_gettop(L);
        luaL_setfuncs(L, audioMetaLib, 0);

        // meta table = methods
        luaL_newlib(L, audioLib);
        lua_setfield(L, meta_id, "__index");

        // meta table.metatable = metatable
        luaL_newlib(L, audioMetaLib);
        lua_setfield(L, meta_id, "__metatable");

        // class.metatable = metatable
        lua_setmetatable(L, lib_id);

        // AudioClip
        lua_setglobal(L, "AudioClip");
    }
}

namespace Modules::Rendering {
    using namespace Stardust_Celeste;

    SDC_LAPI _colorcreate(_L) {
        int argc = lua_gettop(L);
        if (argc != 4)
            return luaL_error(L, "Error: Color.create() takes 4 arguments.");

        int r = luaL_checkinteger(L, 1);
        int g = luaL_checkinteger(L, 2);
        int b = luaL_checkinteger(L, 3);
        int a = luaL_checkinteger(L, 4);

        GI::Color color{};
        color.rgba.r = r;
        color.rgba.g = g;
        color.rgba.b = b;
        color.rgba.a = a;

        lua_pushinteger(L, color.color);
        return 1;
    }

    static const luaL_Reg colorLib[] = {
            {"create", _colorcreate},
            {0, 0}
    };

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
        if (argc != 4 && argc != 1) {
            return luaL_error(L, "Error: RenderContext.clearColor() takes 1 or 4 arguments.");
        }

        auto rctx = &(Stardust_Celeste::Rendering::RenderContext::get());

        if(argc == 1) {
            auto col = luaL_checkinteger(L, 1);
            GI::Color c;
            c.color = col;
            rctx->set_color(c);
        } else {
            u8 r = luaL_checkinteger(L, 1);
            u8 g = luaL_checkinteger(L, 2);
            u8 b = luaL_checkinteger(L, 3);
            u8 a = luaL_checkinteger(L, 4);

            rctx->set_color({r, g, b, a});
        }

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
            return luaL_error(L, "Error: RenderContext.matrixOrtho() takes 6 arguments.");
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
        registerLibrary("Color", colorLib);
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

namespace Modules::Graphics {

    SDC_LAPI _meshcreate(_L) {
        int argc = lua_gettop(L);
        if (argc != 0)
            return luaL_error(L, "Error: Mesh.create() takes 0 arguments.");

        void* pptr_location = lua_newuserdata(L, sizeof(Stardust_Celeste::Rendering::Camera*));
        auto pptr = static_cast<Stardust_Celeste::Rendering::Mesh<GI::Vertex>**>(pptr_location);
        *pptr = new Stardust_Celeste::Rendering::Mesh<GI::Vertex>();

        luaL_getmetatable(L, "Mesh");
        lua_setmetatable(L, -2);

        return 1;
    }

    Stardust_Celeste::Rendering::Mesh<GI::Vertex>** getMesh(lua_State* L){
        return static_cast<Stardust_Celeste::Rendering::Mesh<GI::Vertex>**>(luaL_checkudata(L, 1, "Mesh"));
    }

    SDC_LAPI _meshdestroy(_L) {
        auto mesh = getMesh(L);
        delete *mesh;

        return 0;
    }

    SDC_LAPI _meshsetup(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Mesh.setupBuffer() takes 1 argument.");

        auto mesh = *getMesh(L);
        mesh->setup_buffer();

        return 0;
    }

    SDC_LAPI _meshclear(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Mesh.clearData() takes 1 argument.");

        auto mesh = *getMesh(L);
        mesh->clear_data();

        return 0;
    }

    SDC_LAPI _meshdelete(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Mesh.deleteData() takes 1 argument.");

        auto mesh = *getMesh(L);
        mesh->delete_data();

        return 0;
    }

    SDC_LAPI _meshdraw(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Mesh.draw() takes 1 argument.");

        auto mesh = *getMesh(L);
        mesh->draw();

        return 0;
    }

    SDC_LAPI _meshaddvert(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Mesh.draw() takes 7 arguments.");

        auto mesh = *getMesh(L);
        auto x = luaL_checknumber(L, 2);
        auto y = luaL_checknumber(L, 3);
        auto z = luaL_checknumber(L, 4);

        GI::Color col;
        col.color = luaL_checkinteger(L, 5);

        auto u = luaL_checknumber(L, 6);
        auto v = luaL_checknumber(L, 7);

        GI::Vertex vert;
        vert.x = x;
        vert.y = y;
        vert.z = z;
        vert.color = col;
        vert.u = u;
        vert.v = v;

        mesh->vertices.push_back(vert);

        return 0;
    }

    SDC_LAPI _meshaddindex(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Mesh.draw() takes 2 arguments.");

        auto mesh = *getMesh(L);
        auto x = luaL_checkinteger(L, 2);
        mesh->indices.push_back(x);

        return 0;
    }

    static const luaL_Reg meshLib[] = {
            {"create", _meshcreate},
            {"destroy", _meshdestroy},
            {"setupBuffer", _meshsetup},
            {"clearData", _meshclear},
            {"deleteData", _meshdelete},
            {"draw", _meshdraw},
            {"addVertex", _meshaddvert},
            {"addIndex", _meshaddindex},
            {0, 0}
    };

    static const luaL_Reg meshMetaLib[] = {
            {"__gc", _meshdestroy},
            {0,0}
    };

    void initialize_mesh() {
        auto L = reinterpret_cast<lua_State *>(Stardust_Celeste::Scripting::LuaContext::get().lua_context);

        int lib_id, meta_id;

        // new class = {}
        lua_createtable(L, 0, 0);
        lib_id = lua_gettop(L);

        // meta table = {}
        luaL_newmetatable(L, "Mesh");
        meta_id = lua_gettop(L);
        luaL_setfuncs(L, meshMetaLib, 0);

        // meta table = methods
        luaL_newlib(L, meshLib);
        lua_setfield(L, meta_id, "__index");

        // meta table.metatable = metatable
        luaL_newlib(L, meshMetaLib);
        lua_setfield(L, meta_id, "__metatable");

        // class.metatable = metatable
        lua_setmetatable(L, lib_id);

        // Camera
        lua_setglobal(L, "Mesh");

    }

    SDC_LAPI _tmapcreate(_L) {
        int argc = lua_gettop(L);
        if (argc != 3)
            return luaL_error(L, "Error: Tilemap.create() takes 3 arguments.");

        void* pptr_location = lua_newuserdata(L, sizeof(Stardust_Celeste::Graphics::G2D::Tilemap*));
        auto pptr = static_cast<Stardust_Celeste::Graphics::G2D::Tilemap**>(pptr_location);

        auto tex = luaL_checkinteger(L, 1);
        auto x = luaL_checkinteger(L, 2);
        auto y = luaL_checkinteger(L, 3);

        *pptr = new Stardust_Celeste::Graphics::G2D::Tilemap(tex, {static_cast<float>(x), static_cast<float>(y)});

        luaL_getmetatable(L, "Tilemap");
        lua_setmetatable(L, -2);

        return 1;
    }


    Stardust_Celeste::Graphics::G2D::Tilemap** getTMap(lua_State* L){
        return static_cast<Stardust_Celeste::Graphics::G2D::Tilemap**>(luaL_checkudata(L, 1, "Tilemap"));
    }

    SDC_LAPI _tmapdestroy(_L) {
        auto tmap = getTMap(L);
        delete *tmap;
        return 0;
    }

    SDC_LAPI _tmapclear(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Tilemap.clear() takes 1 argument.");


        auto tmap = *getTMap(L);
        tmap->clear_tiles();

        return 0;
    }

    SDC_LAPI _tmapgenerate(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Tilemap.generateMap() takes 1 argument.");


        auto tmap = *getTMap(L);
        tmap->generate_map();

        return 0;
    }

    SDC_LAPI _tmapdraw(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Tilemap.draw() takes 1 argument.");


        auto tmap = *getTMap(L);
        tmap->draw();

        return 0;
    }

    SDC_LAPI _tmapupdate(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Tilemap.update() takes 2 arguments.");

        auto dt = luaL_checknumber(L, 2);

        auto tmap = *getTMap(L);
        tmap->update(dt);

        return 0;
    }

    SDC_LAPI _tmapaddtile(_L) {
        int argc = lua_gettop(L);
        if (argc != 1)
            return luaL_error(L, "Error: Tilemap.addTile() takes 9 arguments.");

        auto tmap = *getTMap(L);

        Stardust_Celeste::Graphics::G2D::Tile tile;
        tile.bounds.position.x = luaL_checknumber(L, 2);
        tile.bounds.position.y = luaL_checknumber(L, 3);
        tile.bounds.extent.x = luaL_checknumber(L, 4);
        tile.bounds.extent.y = luaL_checknumber(L, 5);
        tile.index = luaL_checkinteger(L, 6);
        tile.color.color = luaL_checkinteger(L, 7);
        tile.layer = luaL_checknumber(L, 8);

        tmap->add_tile(tile);

        return 0;
    }

    static const luaL_Reg tmapLib[] = {
            {"create", _tmapcreate},
            {"destroy", _tmapdestroy},
            {"addTile", _tmapaddtile},
            {"clear", _tmapclear},
            {"generateMap", _tmapgenerate},
            {"update", _tmapupdate},
            {"draw", _tmapdraw},
            {0, 0}
    };

    static const luaL_Reg tmapMetaLib[] = {
            {"__gc", _tmapdestroy},
            {0,0}
    };

    void initialize_tilemap() {
        auto L = reinterpret_cast<lua_State *>(Stardust_Celeste::Scripting::LuaContext::get().lua_context);

        int lib_id, meta_id;

        // new class = {}
        lua_createtable(L, 0, 0);
        lib_id = lua_gettop(L);

        // meta table = {}
        luaL_newmetatable(L, "Tilemap");
        meta_id = lua_gettop(L);
        luaL_setfuncs(L, tmapMetaLib, 0);

        // meta table = methods
        luaL_newlib(L, tmapLib);
        lua_setfield(L, meta_id, "__index");

        // meta table.metatable = metatable
        luaL_newlib(L, tmapMetaLib);
        lua_setfield(L, meta_id, "__metatable");

        // class.metatable = metatable
        lua_setmetatable(L, lib_id);

        // Camera
        lua_setglobal(L, "Tilemap");

    }

    SDC_LAPI _cameracreate(_L) {
        int argc = lua_gettop(L);
        if (argc != 4)
            return luaL_error(L, "Error: Camera.create() takes 4 arguments.");

        float x = luaL_checknumber(L, 1);
        float y = luaL_checknumber(L, 2);
        float z = luaL_checknumber(L, 3);
        float w = luaL_checknumber(L, 4);

        void* pptr_location = lua_newuserdata(L, sizeof(Stardust_Celeste::Rendering::Camera*));
        Stardust_Celeste::Rendering::Camera** pptr = static_cast<Stardust_Celeste::Rendering::Camera**>(pptr_location);
        *pptr = new Stardust_Celeste::Rendering::Camera({0, 0, 0}, {0, 0, 0}, x, y, z, w);

        luaL_getmetatable(L, "Camera");
        lua_setmetatable(L, -2);

        return 1;
    }

    Stardust_Celeste::Rendering::Camera** getCam(lua_State* L){
        return (Stardust_Celeste::Rendering::Camera**)luaL_checkudata(L, 1, "Camera");
    }


    SDC_LAPI _cameradestroy(_L) {
        auto cam = *getCam(L);
        delete cam;

        return 0;
    }

    SDC_LAPI _camerasetpos(_L) {
        int argc = lua_gettop(L);
        if (argc != 4)
            return luaL_error(L, "Error: Camera.setPosition() takes 4 arguments.");

        auto cam = *getCam(L);
        auto x = luaL_checknumber(L, 2);
        auto y = luaL_checknumber(L, 3);
        auto z = luaL_checknumber(L, 4);

        cam->pos = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};

        return 0;
    }
    SDC_LAPI _camerasetrot(_L) {
        int argc = lua_gettop(L);
        if (argc != 4)
            return luaL_error(L, "Error: Camera.setRotation() takes 4 arguments.");

        auto cam = *getCam(L);
        auto x = luaL_checknumber(L, 2);
        auto y = luaL_checknumber(L, 3);
        auto z = luaL_checknumber(L, 4);

        cam->rot = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};

        return 0;
    }

    SDC_LAPI _camerasetproj(_L) {
        int argc = lua_gettop(L);
        if (argc != 5)
            return luaL_error(L, "Error: Camera.setProjection() takes 5 arguments.");

        auto cam = *getCam(L);
        auto x = luaL_checknumber(L, 2);
        auto y = luaL_checknumber(L, 3);
        auto z = luaL_checknumber(L, 4);
        auto w = luaL_checknumber(L, 5);

        cam->set_proj(x, y, z, w);

        return 0;
    }

    static const luaL_Reg camLib[] = {
            {"create", _cameracreate},
	        {"destroy", _cameradestroy},
	        {"setRotation", _camerasetrot},
	        {"setPosition", _camerasetpos},
            {"setProjection", _camerasetproj},
            {0, 0}
    };

    static const luaL_Reg cameraMetaLib[] = {
	        {"__gc", _cameradestroy},
	        {0,0}
    };

    void initialize_camera() {
        auto L = reinterpret_cast<lua_State*>(Stardust_Celeste::Scripting::LuaContext::get().lua_context);

        int lib_id, meta_id;

        // new class = {}
        lua_createtable(L, 0, 0);
        lib_id = lua_gettop(L);

        // meta table = {}
        luaL_newmetatable(L, "Camera");
        meta_id = lua_gettop(L);
        luaL_setfuncs(L, cameraMetaLib, 0);

        // meta table = methods
        luaL_newlib(L, camLib);
        lua_setfield(L, meta_id, "__index");

        // meta table.metatable = metatable
        luaL_newlib(L, cameraMetaLib);
        lua_setfield(L, meta_id, "__metatable");

        // class.metatable = metatable
        lua_setmetatable(L, lib_id);

        // Camera
        lua_setglobal(L, "Camera");
    }

    SDC_LAPI _spritecreate(_L) {
        int argc = lua_gettop(L);
        if (argc != 3)
            return luaL_error(L, "Error: Sprite.create() takes 3 arguments.");

        u32 tex = luaL_checkinteger(L, 1);
        float sx = luaL_checknumber(L, 2);
        float sy = luaL_checknumber(L, 3);

        void* pptr_location = lua_newuserdata(L, sizeof(Stardust_Celeste::Graphics::G2D::Sprite*));
        Stardust_Celeste::Graphics::G2D::Sprite** pptr = static_cast<Stardust_Celeste::Graphics::G2D::Sprite**>(pptr_location);
        *pptr = new Stardust_Celeste::Graphics::G2D::Sprite(tex, {{0, 0}, {sx, sy}});

        luaL_getmetatable(L, "Sprite");
        lua_setmetatable(L, -2);

        return 1;
    }

    SDC_LAPI _spritecreatea(_L) {
        int argc = lua_gettop(L);
        if (argc != 3)
            return luaL_error(L, "Error: Sprite.createAnimated() takes 5 arguments.");

        u32 tex = luaL_checkinteger(L, 1);
        float sx = luaL_checknumber(L, 2);
        float sy = luaL_checknumber(L, 3);
        float su = luaL_checknumber(L, 4);
        float sv = luaL_checknumber(L, 5);

        void* pptr_location = lua_newuserdata(L, sizeof(Stardust_Celeste::Graphics::G2D::Sprite*));
        Stardust_Celeste::Graphics::G2D::Sprite** pptr = static_cast<Stardust_Celeste::Graphics::G2D::Sprite**>(pptr_location);
        *pptr = new Stardust_Celeste::Graphics::G2D::AnimatedSprite(tex, {{0, 0}, {sx, sy}}, {su, sv});

        luaL_getmetatable(L, "Sprite");
        lua_setmetatable(L, -2);

        return 1;
    }

    Stardust_Celeste::Graphics::G2D::Sprite** getSprite(lua_State* L){
        return (Stardust_Celeste::Graphics::G2D::Sprite**)luaL_checkudata(L, 1, "Sprite");
    }

    SDC_LAPI _spritedestroy(_L) {
        auto sprite = getSprite(L);
        delete (*sprite);

        return 0;
    }

    SDC_LAPI _spriteupdate(_L) {
        int argc = lua_gettop(L);
        if(argc != 2)
            return luaL_error(L, "Error: Sprite.update() takes 2 arguments.");

        auto sprite = *getSprite(L);

        auto dt = luaL_checknumber(L, 2);
        sprite->update(dt);

        return 0;
    }

    SDC_LAPI _spritedraw(_L) {
        int argc = lua_gettop(L);
        if(argc != 1)
            return luaL_error(L, "Error: Sprite.draw() takes 1 argument.");

        auto sprite = *getSprite(L);
        sprite->draw();

        return 0;
    }

    SDC_LAPI _spritesetpos(_L) {
        int argc = lua_gettop(L);
        if(argc != 3)
            return luaL_error(L, "Error: Sprite.setPosition() takes 3 arguments.");

        auto x = luaL_checknumber(L, 2);
        auto y = luaL_checknumber(L, 3);

        auto sprite = *getSprite(L);
        sprite->set_position({static_cast<float>(x), static_cast<float>(y)});

        return 0;
    }

    SDC_LAPI _spritesetsize(_L) {
        int argc = lua_gettop(L);
        if(argc != 3)
            return luaL_error(L, "Error: Sprite.setSize() takes 3 arguments.");

        auto x = luaL_checknumber(L, 2);
        auto y = luaL_checknumber(L, 3);

        auto sprite = *getSprite(L);
        sprite->set_size({static_cast<float>(x), static_cast<float>(y)});

        return 0;
    }

    SDC_LAPI _spritesetlayer(_L) {
        int argc = lua_gettop(L);
        if(argc != 2)
            return luaL_error(L, "Error: Sprite.setLayer() takes 2 argument.");

        auto x = luaL_checkinteger(L, 2);

        auto sprite = *getSprite(L);
        sprite->set_layer(x);

        return 0;
    }

    SDC_LAPI _spritesetcolor(_L) {
        int argc = lua_gettop(L);
        if(argc != 2)
            return luaL_error(L, "Error: Sprite.setColor() takes 2 argument.");

        auto x = luaL_checkinteger(L, 2);

        auto sprite = *getSprite(L);

        GI::Color c;
        c.color = x;

        sprite->set_color(c);
        return 0;
    }


    SDC_LAPI _spritesettexsel(_L) {
        int argc = lua_gettop(L);
        if(argc != 5)
            return luaL_error(L, "Error: Sprite.setTexSel() takes 5 arguments.");

        auto x = static_cast<float>(luaL_checknumber(L, 2));
        auto y = static_cast<float>(luaL_checknumber(L, 3));
        auto w = static_cast<float>(luaL_checknumber(L, 4));
        auto h = static_cast<float>(luaL_checknumber(L, 5));

        auto sprite = *getSprite(L);
        sprite->set_selection({ {x, y}, {w, h}});

        return 0;
    }

    SDC_LAPI _spritetick(_L) {
        int argc = lua_gettop(L);
        if(argc != 1)
            return luaL_error(L, "Error: Sprite.update() takes 1 argument.");

        auto sprite = static_cast<Stardust_Celeste::Graphics::G2D::AnimatedSprite*>(*getSprite(L));
        sprite->tick();

        return 0;
    }

    SDC_LAPI _spritetickrate(_L) {
        int argc = lua_gettop(L);
        if(argc != 2)
            return luaL_error(L, "Error: Sprite.setTickRate() takes 2 arguments.");

        auto sprite = static_cast<Stardust_Celeste::Graphics::G2D::AnimatedSprite*>(*getSprite(L));

        auto x = luaL_checknumber(L, 2);
        sprite->ticksPerSec = x;

        return 0;
    }

    static const luaL_Reg spriteLib[] = {
            {"create", _spritecreate},
            {"createAnimated", _spritecreatea},
            {"destroy", _spritedestroy},
            {"update", _spriteupdate},
            {"tick", _spritetick},
            {"setTickRate", _spritetickrate},
            {"draw", _spritedraw},
            {"setPosition", _spritesetpos},
            {"setSize", _spritesetsize},
            {"setLayer", _spritesetlayer},
            {"setColor", _spritesetcolor},
            {"setTexSel", _spritesettexsel},
            {0,0}
    };

    static const luaL_Reg spriteMetaLib[] = {
            {"__gc", _spritedestroy},
            {0,0}
    };

    void initialize_sprites() {
        auto L = reinterpret_cast<lua_State*>(Stardust_Celeste::Scripting::LuaContext::get().lua_context);

        int lib_id, meta_id;

        // new class = {}
        lua_createtable(L, 0, 0);
        lib_id = lua_gettop(L);

        // meta table = {}
        luaL_newmetatable(L, "Sprite");
        meta_id = lua_gettop(L);
        luaL_setfuncs(L, spriteMetaLib, 0);

        // meta table = methods
        luaL_newlib(L, spriteLib);
        lua_setfield(L, meta_id, "__index");

        // meta table.metatable = metatable
        luaL_newlib(L, spriteMetaLib);
        lua_setfield(L, meta_id, "__metatable");

        // class.metatable = metatable
        lua_setmetatable(L, lib_id);

        // AudioClip
        lua_setglobal(L, "Sprite");
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
        Modules::Audio::initialize_audio();

        Modules::Rendering::initialize_rendering();
        Modules::Rendering::initialize_texture();
        Modules::Graphics::initialize_camera();
        Modules::Graphics::initialize_mesh();
        Modules::Graphics::initialize_tilemap();

        Modules::Utilities::initialize_utils();

        Modules::Graphics::initialize_sprites();
    }

    auto LuaContext::cleanup() -> void {
        lua_close(reinterpret_cast<lua_State*>(lua_context));
    }
}