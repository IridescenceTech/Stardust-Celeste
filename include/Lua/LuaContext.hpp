#pragma once
#include <Utilities/Singleton.hpp>

namespace Stardust_Celeste::Scripting {

    class LuaContext : public Singleton {
    public:
        LuaContext() = default;
        ~LuaContext();

        auto init() -> void;
        auto cleanup() -> void;

        inline static auto get() -> LuaContext & {
            static LuaContext lctx;
            return lctx;
        }

        inline static auto getContext() -> void * {
            return get().lua_context;
        }

    private:
        void* lua_context = nullptr;
    };
}