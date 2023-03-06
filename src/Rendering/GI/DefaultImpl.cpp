#if PSP
#define GUGL_IMPLEMENTATION
#endif

#include <Rendering/GI.hpp>
#include <Utilities/Assertion.hpp>
#include <string>

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.hpp"

#include <Rendering/GI/GL/GLTextureHandle.hpp>
#include <Rendering/GI/VK/VkTextureHandle.hpp>
#include "Core/Application.hpp"
#include "Rendering/GI/GL/GLBufferObject.hpp"
#include "Rendering/GI/VK/VkBufferObject.hpp"
#include "Rendering/GI/VK/VkContext.hpp"
#include "Rendering/GI/VK/VkPipeline.hpp"

#include "Rendering/GI/DX11/DXContext.hpp"
#elif BUILD_PLAT == BUILD_PSP
#define GUGL_IMPLEMENTATION
#include <gu2gl/gu2gl.h>
#include <pspctrl.h>
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
#elif BUILD_PLAT == BUILD_3DS
#include <GL/picaGL.h>
#endif

#if BUILD_PC
const std::string vert_source = R"(
    #version 400
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aCol;
    layout (location = 2) in vec2 aTex;

    layout (std140) uniform Matrices {
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
    };

    out vec2 uv;
    out vec4 color;
    out vec3 position;

    void main() {
        gl_Position = proj * view * model * vec4(aPos, 1.0);
        position = gl_Position.xyz;
        uv = aTex;
        color = aCol;
    }
)";

const std::string frag_source = R"(
    #version 400
    uniform sampler2D tex;
    uniform int noTex;
    uniform float scroll;
    uniform int fog;

    const vec3 fogColor = vec3(0.6f, 0.8f, 0.9f);

    in vec2 uv;
    in vec4 color;
    in vec3 position;

    out vec4 FragColor;

    float Convert_sRGB_ToLinear (float thesRGBValue) {
        return thesRGBValue <= 0.04045f
                ? thesRGBValue / 12.92f
                : pow ((thesRGBValue + 0.055f) / 1.055f, 2.4f);
    }

    void main() {
        vec4 texColor = texture(tex, vec2(uv.x + scroll, uv.y));
        vec4 color1 = color * vec4(1.0f / 255.0f);
        vec4 color2 = vec4(Convert_sRGB_ToLinear(color1.r), Convert_sRGB_ToLinear(color1.g), Convert_sRGB_ToLinear(color1.b), color1.a);
        if(noTex == 0) {
            texColor *= color2;
        } else if(noTex == 1) {
            texColor = color2;
        }

        if(fog == 1) {
            float dist = abs(position.z);
            const float fogMax = (256.0f * 0.8);
            const float fogMin = (256.0f * 0.2);
            float fogFactor = (fogMax - dist) / (fogMax - fogMin);
            fogFactor = clamp(fogFactor, 0.0f, 1.0f);
            texColor = vec4(mix(fogColor.rgb, texColor.rgb, fogFactor), texColor.a);
        }

        FragColor = texColor;

        if(FragColor.a < 0.1f)
            discard;

   }
)";

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

#elif BUILD_VITA

const std::string vert_source =
        R"(
void main(float3 position, float4 color, float2 uv,
    float2 out vTexcoord : TEXCOORD0, float4 out vPosition : POSITION, float4 out vColor : COLOR,
    uniform float4x4 proj, uniform float4x4 view, uniform float4x4 model){
        vPosition = mul(mul(mul(float4(position, 1.f), model), view), proj);
        vTexcoord = uv;
        vColor = color;
    }
)";

const std::string frag_source =
        R"(
float4 main(float2 vTexcoord : TEXCOORD0, float4 vColor : COLOR0, uniform sampler2D tex) {

    float4 texColor = tex2D(tex, vTexcoord);
    texColor *= vColor;
    texColor = clamp(texColor, 0.0f, 1.0f);

    if(texColor.a < 0.1f)
        discard;

    return texColor;
}
)";

#endif


namespace GI {
    RenderContextSettings rctxSettings;

#if BUILD_PC
    GLFWwindow *window;
    GLuint programID;
    GLuint ubo;
    u32 projLoc, viewLoc, modLoc;
    u32 noTex, scroll;

    namespace detail {
        extern GLFWwindow* window;
    }

#ifndef NO_EXPERIMENTAL_GRAPHICS
    D3DXCOLOR clearColor;
#endif

#elif BUILD_PLAT == BUILD_PSP
    unsigned int __attribute__((aligned(16))) list[0x10000];
#elif BUILD_PLAT == BUILD_VITA
    GLuint programID;
#endif


#if BUILD_PC || BUILD_PLAT == BUILD_VITA

    auto compileShader(const char *source, GLenum shaderType) -> GLuint {
        auto shader = glCreateShader(shaderType);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (!status) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::string err = "Shader compile failed: " + std::string(log);
            SC_CORE_ERROR("SHADER COMPILE FAILED: {}", err);
            throw std::runtime_error(err);
        }

        return shader;
    }

    auto linkShaders(GLuint vshader, GLuint fshader) -> GLuint {
        auto prog = glCreateProgram();
        glAttachShader(prog, vshader);
        glAttachShader(prog, fshader);

#if BUILD_PLAT == BUILD_VITA
        glBindAttribLocation(prog, 0, "position");
        glBindAttribLocation(prog, 1, "color");
        glBindAttribLocation(prog, 2, "uv");
#endif

        glLinkProgram(prog);

        GLint status = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &status);

        if (!status) {
            char log[512];
            glGetProgramInfoLog(prog, 512, nullptr, log);
            throw std::runtime_error("Shader linking failed: " + std::string(log));
        }

        return prog;
    }

    auto loadShaders(const std::string& vs, const std::string& fs) -> GLuint {
        GLuint vertShader, fragShader;

        vertShader = compileShader(vs.c_str(), GL_VERTEX_SHADER);
        fragShader = compileShader(fs.c_str(), GL_FRAGMENT_SHADER);

        auto pID = linkShaders(vertShader, fragShader);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        glUseProgram(pID);

        return pID;
    }
#endif

    auto init(const RenderContextSettings app) -> void {
        rctxSettings = app;

#if BUILD_PC
        if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::VKContext::get().init(app);
            window = detail::window;
            detail::VKPipeline::get().init();
#endif
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            SC_CORE_ASSERT(glfwInit(), "GLFW Init Failed!");

            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

            window = glfwCreateWindow(app.width, app.height, app.title, nullptr, nullptr);
            glfwMakeContextCurrent(window);
            glfwSwapInterval(0);

            SC_CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
                           "OpenGL Init Failed!");
        } else if (rctxSettings.renderingApi == DX11) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            SC_CORE_ASSERT(glfwInit(), "GLFW Init Failed!");

            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            window = glfwCreateWindow(app.width, app.height, app.title, nullptr, nullptr);
            glfwSwapInterval(0);

            detail::DXContext::get().init(window);
#endif
        }

#elif BUILD_PLAT == BUILD_PSP
        guglInit(list);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumOrtho(-1, 1, -1, 1, -1, 1);
        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();

        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
#elif BUILD_PLAT == BUILD_VITA
        vglInit(0x800000);
        vglWaitVblankStart(GL_TRUE);
#elif BUILD_PLAT == BUILD_3DS
        pglInitEx(0x040000, 0x1000000);
        pglSelectScreen(GFX_TOP, 0);
#endif

        // Extended setup: Vita / Desktop Shaders
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            GI::programID = loadShaders(vert_source, frag_source);
            glUseProgram(GI::programID);

            noTex = glGetUniformLocation(GI::programID, "noTex");
            scroll = glGetUniformLocation(GI::programID, "scroll");

#if BUILD_PLAT != BUILD_VITA
            GLuint ubi = glGetUniformBlockIndex(GI::programID, "Matrices");
            glUniformBlockBinding(GI::programID, ubi, 0);
            glGenBuffers(1, &ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(Math::Matrix), NULL,
                         GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 3 * sizeof(Math::Matrix));
            glBindBuffer(GL_UNIFORM_BUFFER, GI::programID);
            glEnable(GL_FRAMEBUFFER_SRGB);
#else
            projLoc = glGetUniformLocation(GI::programID, "proj");
            viewLoc = glGetUniformLocation(GI::programID, "view");
            modLoc = glGetUniformLocation(GI::programID, "model");
#endif
        }
#endif
    }
    auto terminate() -> void {
#if BUILD_PC
        if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::VKPipeline::get().deinit();
            detail::VKContext::get().deinit();
#endif
        } else if (rctxSettings.renderingApi == DX11) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::DXContext::get().deinit();
#endif
        }
        glfwDestroyWindow(window);
        glfwTerminate();
#elif BUILD_PLAT == BUILD_PSP
        guglTerm();
#elif BUILD_PLAT == BUILD_VITA
        vglEnd();
#elif BUILD_PLAT == BUILD_3DS
        pglExit();
#endif
    }

    auto enable(u32 state) -> void {
        if (rctxSettings.renderingApi == Vulkan) {
            if(state == GI_DEPTH_TEST) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
                vkCmdSetDepthTestEnable(detail::VKPipeline::get().commandBuffer, true);
#endif
            }
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            glEnable(state);
        }
    }
    auto disable(u32 state) -> void {
        if(rctxSettings.renderingApi == Vulkan) {
            if(state == GI_DEPTH_TEST) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
                vkCmdSetDepthTestEnable(detail::VKPipeline::get().commandBuffer, false);
#endif
            }
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#ifndef PSP
            if (state == GI_TEXTURE_2D)
                glBindTexture(GL_TEXTURE_2D, 0);
#endif
            glDisable(state);
        }
    }

    auto set_culling_mode(bool enabled, bool ccw) -> void {
        if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            VkCullModeFlags cullMode;
            if(enabled) {
                if(!ccw) {
                    cullMode = VK_CULL_MODE_FRONT_BIT;
                } else {
                    cullMode = VK_CULL_MODE_BACK_BIT;
                }
            } else {
                cullMode = VK_CULL_MODE_NONE;
            }

            vkCmdSetCullMode(detail::VKPipeline::get().commandBuffer, cullMode);
#endif
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            if (enabled) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }
#ifndef PSP
            glCullFace(GL_BACK);
#endif
            glFrontFace(ccw ? GL_CCW : GL_CW);
        }
    }

    auto depth_func(u32 mode) -> void {
        if (rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            glDepthFunc(mode);
        }
    }

    auto blend_func(u32 src, u32 dest) -> void {
        if (rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            VkColorBlendEquationEXT blendEquationExt;


            VkBlendFactor src2, dest2;
            if(src == GI_SRC_COLOR) {
                src2 = VK_BLEND_FACTOR_SRC_COLOR;
            } else if(src == GI_ONE_MINUS_SRC_COLOR) {
                src2 = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            } else if(src == GI_SRC_ALPHA) {
                src2 = VK_BLEND_FACTOR_SRC_ALPHA;
            } else if(src == GI_ONE_MINUS_SRC_ALPHA) {
                src2 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            } else if(src == GI_DST_COLOR) {
                src2 = VK_BLEND_FACTOR_DST_COLOR;
            } else if(src == GI_ONE_MINUS_DST_COLOR) {
                src2 = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            } else if(src == GI_DST_ALPHA) {
                src2 = VK_BLEND_FACTOR_DST_ALPHA;
            } else if(src == GI_ONE_MINUS_DST_ALPHA) {
                src2 = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            } else {
                src2 = VK_BLEND_FACTOR_SRC_COLOR;
            }


            if(dest == GI_SRC_COLOR) {
                dest2 = VK_BLEND_FACTOR_SRC_COLOR;
            } else if(dest == GI_ONE_MINUS_SRC_COLOR) {
                dest2 = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            } else if(dest == GI_SRC_ALPHA) {
                dest2 = VK_BLEND_FACTOR_SRC_ALPHA;
            } else if(dest == GI_ONE_MINUS_SRC_ALPHA) {
                dest2 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            } else if(dest == GI_DST_COLOR) {
                dest2 = VK_BLEND_FACTOR_DST_COLOR;
            } else if(dest == GI_ONE_MINUS_DST_COLOR) {
                dest2 = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            } else if(dest == GI_DST_ALPHA) {
                dest2 = VK_BLEND_FACTOR_DST_ALPHA;
            } else if(dest == GI_ONE_MINUS_DST_ALPHA) {
                dest2 = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            } else {
                dest2 = VK_BLEND_FACTOR_SRC_COLOR;
            }

            blendEquationExt.srcColorBlendFactor = src2;
            blendEquationExt.dstColorBlendFactor = dest2;
            blendEquationExt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            blendEquationExt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blendEquationExt.colorBlendOp = VK_BLEND_OP_ADD;
            blendEquationExt.alphaBlendOp = VK_BLEND_OP_ADD;

            auto fn = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetDeviceProcAddr(detail::VKContext::get().logicalDevice, "vkCmdSetColorBlendEquationEXT"));
            fn(detail::VKPipeline::get().commandBuffer, 0, 1, &blendEquationExt);
#endif
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#ifndef PSP
            glBlendFunc(src, dest);
#else
            glBlendFunc(GU_ADD, src, dest, 0, 0);
#endif
        }
    }

    auto alpha_func(u32 func, u32 value, u32 mask) -> void {
        if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#ifdef PSP
            glAlphaFunc(func, value, mask);
#elif BUILD_PLAT == BUILD_3DS
            glAlphaFunc(func, (float) value / 255.0f);
#endif
        }
    }

    auto start_frame(bool dialog) -> void {
        if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::VKPipeline::get().beginFrame();
#endif
        } else {
#if BUILD_PLAT == BUILD_PSP
            guglStartFrame(list, dialog);
#endif
        }
    }

    auto end_frame(bool vsync, bool dialog) -> void {
#if BUILD_PC
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        if(time > 1.0f / 144.0f){
            glfwPollEvents();
            startTime = currentTime;
        }

        if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::VKPipeline::get().endFrame();
#endif
        } else if (rctxSettings.renderingApi == DX11) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::DXContext::get().swapChain->Present(0, 0);
#endif
        }

        if (vsync)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        if (glfwWindowShouldClose(window))
            Stardust_Celeste::Core::Application::get().exit();

        glfwSwapBuffers(window);
#elif BUILD_PLAT == BUILD_PSP
        guglSwapBuffers(vsync, dialog);
#elif BUILD_PLAT == BUILD_VITA
        vglSwapBuffers(dialog);
#elif BUILD_PLAT == BUILD_3DS
        pglSwapBuffersEx(true, false);

        if (vsync)
            gspWaitForVBlank();
#endif
    }

    auto to_vec4(Color &c) -> Math::Vector4<float> {
        return {static_cast<float>(c.rgba.r) / 255.0f,
                static_cast<float>(c.rgba.g) / 255.0f,
                static_cast<float>(c.rgba.b) / 255.0f,
                static_cast<float>(c.rgba.a) / 255.0f};
    }

    auto clear_color(Color color) -> void {
        if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::VKPipeline::get().clearColor = to_vec4(color);
#endif
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#if BUILD_PLAT == BUILD_PSP
            glClearColor(color.color);
#elif BUILD_PLAT == BUILD_3DS
            glClearColor(color.rgba.r, color.rgba.b, color.rgba.g, color.rgba.a);
#else
            auto c = to_vec4(color);
            glClearColor(c.x, c.y, c.z, c.w);
#endif
        } else if (rctxSettings.renderingApi == DX11) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            auto c = to_vec4(color);
            clearColor = D3DXCOLOR(c.r, c.g, c.b, c.a);
#endif
        }
    }
    auto clear(u32 mask) -> void {
        if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#if BUILD_PLAT == BUILD_PSP
            glClear(mask | GL_FAST_CLEAR_BIT);
#else
            glClear(mask);
#endif
        } else if(rctxSettings.renderingApi == DX11) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            detail::DXContext::get().clear(clearColor);
#endif
        }
    }

    auto clearDepth() -> void {
        if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            glClear(GL_DEPTH_BUFFER_BIT);
        }
    }

    auto enable_textures() -> void {
        if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#if BUILD_PLAT == BUILD_VITA || BUILD_PC
            glUniform1i(noTex, 0);
#endif
        }
    }

    auto disable_textures() -> void {
        if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#if BUILD_PLAT == BUILD_VITA || BUILD_PC
            glUniform1i(noTex, 1);
#endif
        }
    }

    auto set_tex_scroll(float v) -> void {
        if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
#if BUILD_PLAT == BUILD_VITA || BUILD_PC
            glUniform1f(scroll, v);
#endif
        }
    }

    auto create_texturehandle(std::string filename, u32 magFilter, u32 minFilter, bool repeat, bool flip) -> TextureHandle* {
        if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            return detail::VKTextureHandle::create(filename, magFilter, minFilter, repeat, flip);
#endif
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            return detail::GLTextureHandle::create(filename, magFilter, minFilter, repeat, flip);
        }

        return nullptr;
    }

    auto create_vertexbuffer(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) -> BufferObject* {
        if (rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
            return detail::VKBufferObject::create(vert_data, vert_size, indices, idx_size);
#endif
        } else if(rctxSettings.renderingApi == OpenGL || rctxSettings.renderingApi == DefaultAPI) {
            return detail::GLBufferObject::create(vert_data, vert_size, indices, idx_size);
        }

        return nullptr;
    }
}