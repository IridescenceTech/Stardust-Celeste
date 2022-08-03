#pragma once
#include "RenderTypes.hpp"
#include <Utilities/Singleton.hpp>
#include <map>
#include <string>

#if (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)
#include <glad/glad.hpp>
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
#else
#endif

namespace Stardust_Celeste::Rendering {

#ifndef PSP

struct Shader {
    GLuint programID;
    GLuint ubi;
    GLuint ubo;
    GLuint projLoc;
    GLuint viewLoc;
    GLuint modLoc;
};

class ShaderManager : public Singleton {
  public:
    ShaderManager() = default;
    ~ShaderManager();

    auto load_shader(std::string vs, std::string fs) -> int;
    auto bind_shader(u32 id) -> void;

    inline auto get_shader(u32 id) -> Shader { return fullMap[id]; }
    inline auto get_current_shader() -> Shader {
        return fullMap[current_shader];
    }

    inline static auto get() -> ShaderManager & {
        static ShaderManager smx;
        return smx;
    }

  private:
    std::map<u32, Shader> fullMap;
    u32 shadCount = 0;
    int current_shader = 0;
};
#endif
} // namespace Stardust_Celeste::Rendering