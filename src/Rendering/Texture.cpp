#include <Platform/Platform.hpp>
#include <Rendering/Texture.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.hpp>
#elif BUILD_PLAT == BUILD_PSP
#include <intraFont.h>
#include <malloc.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <psppower.h>
#include <psptypes.h>
#include <psputils.h>
#include <stdarg.h>
#endif

#include <Utilities/Assertion.hpp>

#include <ext/stb_image.hpp>
#include <glm.hpp>
#include <string>
#include <vector>

#if BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
#endif

#if BUILD_PLAT == BUILD_PSP
inline auto swizzle_fast(u8 *out, const u8 *in, unsigned int width,
                         unsigned int height) -> void {
    unsigned int blockx, blocky;
    unsigned int j;

    unsigned int width_blocks = (width / 16);
    unsigned int height_blocks = (height / 8);

    unsigned int src_pitch = (width - 16) / 4;
    unsigned int src_row = width * 8;

    const u8 *ysrc = in;
    u32 *dst = (u32 *)out;

    for (blocky = 0; blocky < height_blocks; ++blocky) {
        const u8 *xsrc = ysrc;
        for (blockx = 0; blockx < width_blocks; ++blockx) {
            const u32 *src = (u32 *)xsrc;
            for (j = 0; j < 8; ++j) {
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                src += src_pitch;
            }
            xsrc += 16;
        }
        ysrc += src_row;
    }
}

u32 offset = 10 * 512 * 272;

#endif

namespace Stardust_Celeste::Rendering {

auto pow2(u32 value) -> u32 {
    u32 poweroftwo = 1;
    while (poweroftwo < value) {
        poweroftwo <<= 1;
    }
    return poweroftwo;
}

auto TextureManager::get_texture(std::string name) -> u32 {

    for (auto &[key, val] : fullMap) {
        if (val->name == name)
            return key;
    }

    return -1;
}

auto TextureManager::load_texture(std::string filename, u32 magFilter,
                                  u32 minFilter, bool repeat, bool flip,
                                  bool vram) -> u32 {

    int width, height, nrChannels;
    if (flip)
        stbi_set_flip_vertically_on_load(true);
    else
        stbi_set_flip_vertically_on_load(false);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height,
                                    &nrChannels, STBI_rgb_alpha);
    SC_CORE_ASSERT(data, "Could not load file: " + filename + "!");

    Texture *tex = new Texture();
    tex->width = width;
    tex->height = height;

    tex->pW = pow2(width);
    tex->pH = pow2(height);

    tex->ramSpace = 0; // TODO: Add option for PSP VRAM
    tex->swizzle = 1;
    tex->colorMode = 3; // GU_PSM_8888
    tex->id = 0;

    tex->repeating = repeat;
    tex->magFilter = magFilter;
    tex->minFilter = minFilter;

    tex->name = filename;

#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    tex->data = nullptr;

    if (repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
#elif BUILD_PLAT == BUILD_PSP
    unsigned int *dataBuffer =
        (unsigned int *)memalign(16, tex->pH * tex->pW * 4);

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            dataBuffer[x + y * tex->pW] = ((unsigned int *)data)[x + y * width];
        }
    }

    stbi_image_free(data);
    tex->data = (uint16_t *)dataBuffer;

    unsigned int *swizzled_pixels = nullptr;
    if (!vram)
        swizzled_pixels = (unsigned int *)memalign(16, tex->pH * tex->pW * 4);
    else {
        swizzled_pixels =
            (unsigned int *)((int)offset + (int)sceGeEdramGetAddr());
        offset += tex->pH * tex->pW * 4;
    }

    swizzle_fast((u8 *)swizzled_pixels, (const u8 *)dataBuffer, tex->pW * 4,
                 tex->pH);

    free(dataBuffer);
    tex->data = (u16 *)swizzled_pixels;

    sceKernelDcacheWritebackInvalidateAll();
#endif

    fullMap.emplace(texCount, tex);
    return texCount++;
}

auto TextureManager::bind_texture(u32 id) -> void {
    if (fullMap.find(id) != fullMap.end()) {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, fullMap[id]->id);
#elif BUILD_PLAT == BUILD_PSP
        sceGuEnable(GU_TEXTURE_2D);
        Texture *tex = fullMap[id];

        sceGuTexMode(tex->colorMode, 0, 0, tex->swizzle);
        sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
        sceGuTexFilter(tex->minFilter, tex->magFilter);

        auto repeat = tex->repeating ? GU_REPEAT : GU_CLAMP;
        sceGuTexWrap(repeat, repeat);

        sceGuTexImage(0, tex->pW, tex->pH, tex->pW, tex->data);
#endif
    }
}

auto TextureManager::delete_texture(u32 id) -> void {
    if (fullMap.find(id) != fullMap.end()) {

#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        glDeleteTextures(1, &fullMap[id]->id);
#endif

        if (fullMap[id]->data)
            free(fullMap[id]->data);

        delete fullMap[id];
        fullMap.erase(id);
    }
}

TextureManager::~TextureManager() {
    std::vector<u32> ids;
    for (auto &[key, val] : fullMap)
        ids.push_back(key);

    for (auto id : ids)
        delete_texture(id);
}

} // namespace Stardust_Celeste::Rendering