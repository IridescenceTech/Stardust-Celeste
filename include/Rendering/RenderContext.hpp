/**
 * @file RenderContext.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Rendering Context
 * @version 0.1
 * @date 2021-12-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "Utilities/Singleton.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Assertion.hpp"
#include "Utilities/Types.hpp"

#include "RenderTypes.hpp"

namespace Stardust_Celeste::Rendering
{

    struct RenderContextSettings {
        u32 width = 1280;
        u32 height = 720;
        const char* title = "Stardust App";
    };

    class RenderContext : public Singleton {
    public:
    RenderContext() = default;    

    auto initialize(const RenderContextSettings app) -> void;
    auto terminate() -> void;

    auto clear() -> void;
    inline auto setColor(Color color) -> void {
        c = color;
    }
    auto render() -> void;

    inline static auto Get() -> RenderContext & {
        static RenderContext rctx;
        return rctx;
    }

    inline auto initialized() -> bool {
        return is_init;
    }

    private:
    bool is_init = false;
    Color c;
    }; 
} // namespace Stardust_Celeste::Rendering
