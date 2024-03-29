#include <Rendering/GI/GL/GLTextureHandle.hpp>
#include <Rendering/GI.hpp>
#include <stb_image.hpp>

namespace GI::detail {

    void GLTextureHandle::bind() {
#ifndef PSP
        GI::enable(GI_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id);
#endif
    }


    GLTextureHandle* GLTextureHandle::create_ram(uint8_t* buf, size_t len, u32 magFilter, u32 minFilter, bool repeat, bool flip) {
#ifndef PSP
        GLTextureHandle* tex = new GLTextureHandle();

        int texWidth, texHeight, texChannels;
        stbi_set_flip_vertically_on_load(flip);
        stbi_uc* data = stbi_load_from_memory(buf, len, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        glGenTextures(1, (GLuint *)&tex->id);
        glBindTexture(GL_TEXTURE_2D, tex->id);

#if BUILD_PC
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, texWidth, texHeight, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
#endif

#if BUILD_PLAT != BUILD_3DS
        glGenerateMipmap(GL_TEXTURE_2D);
#endif
        stbi_image_free(data);

        if (repeat) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

        return tex;
#endif
    }


    GLTextureHandle* GLTextureHandle::create(std::string filename, u32 magFilter, u32 minFilter, bool repeat, bool flip) {
#ifndef PSP
        GLTextureHandle* tex = new GLTextureHandle();

        int texWidth, texHeight, texChannels;
        stbi_set_flip_vertically_on_load(flip);
        stbi_uc* data = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        glGenTextures(1, (GLuint *)&tex->id);
        glBindTexture(GL_TEXTURE_2D, tex->id);

#if BUILD_PC
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, texWidth, texHeight, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
#endif

#if BUILD_PLAT != BUILD_3DS
        glGenerateMipmap(GL_TEXTURE_2D);
#endif
        stbi_image_free(data);

        if (repeat) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

        return tex;
#endif
    }

    void GLTextureHandle::destroy() {
#ifndef PSP
        glDeleteTextures(1, &id);
#endif
    }
}