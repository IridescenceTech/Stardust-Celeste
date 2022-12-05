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

#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_3DS

#ifndef SDC_VULKAN
/**
 * @brief Shader data structure
 *
 */
struct Shader {
    GLuint programID;
    GLuint ubi;
    GLuint ubo;
    GLuint projLoc;
    GLuint viewLoc;
    GLuint modLoc;
};

/**
 * @brief Desktop + Vita Shader Management
 *
 */
class ShaderManager final : public Singleton {
  public:
    ShaderManager() = default;
    virtual ~ShaderManager();

    /**
     * @brief Create a brand new Shader Program
     *
     * @param vs Vertex Shader Source
     * @param fs Fragment Shader Source
     * @return int
     */
    auto load_shader(std::string vs, std::string fs) -> int;

    /**
     * @brief Bind shader object given an ID
     *
     * @param id ID of shader object
     */
    auto bind_shader(u32 id) -> void;

    /**
     * @brief Get the shader object given an ID
     *
     * @param id ID of shader object
     * @return Shader
     */
    inline auto get_shader(u32 id) -> Shader { return fullMap[id]; }

    /**
     * @brief Get the current shader object
     *
     * @return Shader
     */
    inline auto get_current_shader() -> Shader {
        return fullMap[current_shader];
    }

    /**
     * @brief Gets static instance of the ShaderManager
     *
     * @return ShaderManager& Instance
     */
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
#endif
} // namespace Stardust_Celeste::Rendering