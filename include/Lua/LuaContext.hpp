#pragma once
#include <Utilities/Singleton.hpp>
#include <Utilities/Controllers/ControllerCollection.hpp>

namespace Stardust_Celeste::Scripting {

    /**
     * Lua Context object singleton
     */
    class LuaContext : public Singleton {
    public:
        LuaContext() = default;
        ~LuaContext();

        /**
         * Initialize the Lua Context and open libraries
         */
        auto init(RefPtr<Utilities::Input::ControllerCollection> c) -> void;

        /**
         * Cleanup the Lua Context
         */
        auto cleanup() -> void;

        /**
         * Gets the global lua context
         * @return Lua Context
         */
        inline static auto get() -> LuaContext & {
            static LuaContext lctx;
            return lctx;
        }

        /**
         * The context object (lua_State*)
         */
        void* lua_context = nullptr;
    };
}