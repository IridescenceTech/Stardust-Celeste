/**
 * @file Texture.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief
 * @version 0.1
 * @date 2021-12-28
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "RenderTypes.hpp"
#include <Utilities/Singleton.hpp>
#include <map>
#include <string>

#if (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)
#include <glad/glad.hpp>

#define SC_TEX_FILTER_NEAREST GL_NEAREST
#define SC_TEX_FILTER_LINEAR GL_LINEAR

#elif BUILD_PLAT == BUILD_PSP
#include <pspgu.h>

#define SC_TEX_FILTER_NEAREST GU_NEAREST
#define SC_TEX_FILTER_LINEAR GU_LINEAR
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>

#define SC_TEX_FILTER_NEAREST GL_NEAREST
#define SC_TEX_FILTER_LINEAR GL_LINEAR
#endif

namespace Stardust_Celeste::Rendering {

class TextureManager : public Singleton {
  public:
    TextureManager() = default;
    ~TextureManager();

    auto load_texture(std::string filename, u32 magFilter, u32 minFilter,
                      bool repeat, bool flip = false, bool vram = false) -> u32;

    auto get_texture(std::string name) -> u32;

    auto bind_texture(u32 id) -> void;
    auto delete_texture(u32 id) -> void;

    inline static auto get() -> TextureManager & {
        static TextureManager txm;
        return txm;
    }

    inline auto get_texture(unsigned int i) -> Texture * { return fullMap[i]; }

  private:
    std::map<unsigned int, Texture *> fullMap;
    u32 texCount = 0;
};

} // namespace Stardust_Celeste::Rendering