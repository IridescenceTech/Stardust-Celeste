#ifndef SDC_VULKAN
#if PSP
#define GUGL_IMPLEMENTATION
#endif
#include "Rendering/GI.hpp"
#include <string>

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.hpp"
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

#include "Core/Application.hpp"
#include "Utilities/Assertion.hpp"

namespace GI {
#if BUILD_PC
    GLFWwindow *window;
    GLuint programID;
    GLuint ubo;
    u32 projLoc, viewLoc, modLoc;
    u32 noTex, scroll;
#elif BUILD_PLAT == BUILD_PSP
    unsigned int __attribute__((aligned(16))) list[0x10000];
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
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

    auto loadShaders(std::string vs, std::string fs) -> GLuint {
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
#if BUILD_PC
        SC_CORE_ASSERT(glfwInit(), "GLFW Init Failed!");

        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

        window = glfwCreateWindow(app.width, app.height, app.title, NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSwapInterval(0);

        SC_CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
                       "OpenGL Init Failed!");

        glEnable(GL_FRAMEBUFFER_SRGB);
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

#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        GI::programID = loadShaders(vert_source, frag_source);
        glUseProgram(GI::programID);

        noTex = glGetUniformLocation(GI::programID, "noTex");
        scroll = glGetUniformLocation(GI::programID, "scroll");

#if BUILD_PLAT != BUILD_VITA
        GLuint ubi = glGetUniformBlockIndex(GI::programID, "Matrices");
        glUniformBlockBinding(GI::programID, ubi, 0);
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL,
                     GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 3 * sizeof(glm::mat4));
        glBindBuffer(GL_UNIFORM_BUFFER, GI::programID);
#else
        projLoc = glGetUniformLocation(GI::programID, "proj");
        viewLoc = glGetUniformLocation(GI::programID, "view");
        modLoc = glGetUniformLocation(GI::programID, "model");
#endif
#endif
    }

    auto terminate() -> void {
#if BUILD_PC
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

    auto enable(u32 state) -> void { glEnable(state); }
    auto disable(u32 state) -> void {
#ifndef PSP
        if (state == GI_TEXTURE_2D)
            glBindTexture(GL_TEXTURE_2D, 0);
#endif
        glDisable(state);
    }

    auto set_culling_mode(bool enabled, bool ccw) -> void {
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

    auto depth_func(u32 mode) -> void { glDepthFunc(mode); }

    auto blend_func(u32 src, u32 dest) -> void {
#ifndef PSP
        glBlendFunc(src, dest);
#else
        glBlendFunc(GU_ADD, src, dest, 0, 0);
#endif
    }

    auto alpha_func(u32 func, u32 value, u32 mask) -> void {
#ifdef PSP
        glAlphaFunc(func, value, mask);
#elif BUILD_PLAT == BUILD_3DS
        glAlphaFunc(func, (float)value / 255.0f);
#endif
    }

    auto start_frame(bool dialog) -> void {
#if BUILD_PLAT == BUILD_PSP
        guglStartFrame(list, dialog);
#endif
    }

    auto end_frame(bool vsync, bool dialog) -> void {
#if BUILD_PC
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        if(time > 1.0f / 60.0f){
            glfwPollEvents();
            startTime = currentTime;
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

    auto to_vec4(Color &c) -> glm::vec4 {
        return {static_cast<float>(c.rgba.r) / 255.0f,
                static_cast<float>(c.rgba.g) / 255.0f,
                static_cast<float>(c.rgba.b) / 255.0f,
                static_cast<float>(c.rgba.a) / 255.0f};
    }

    auto clear_color(Color color) -> void {
#if BUILD_PLAT == BUILD_PSP
        glClearColor(color.color);
#elif BUILD_PLAT == BUILD_3DS
        glClearColor(color.rgba.r, color.rgba.b, color.rgba.g, color.rgba.a);
#else
        auto c = to_vec4(color);
        glClearColor(c.r, c.g, c.b, c.a);
#endif
    }
    auto clear(u32 mask) -> void {
#if BUILD_PLAT == BUILD_PSP
        glClear(mask | GL_FAST_CLEAR_BIT);
#else
        glClear(mask);
#endif
    }

    auto clearDepth() -> void {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    auto enable_textures() -> void {
        glUniform1i(noTex, 0);
    }
    auto disable_textures() -> void {
        glUniform1i(noTex, 1);
    }
    auto set_tex_scroll(float v) -> void {
        glUniform1f(scroll, v);
    }
} // namespace GI
#endif
