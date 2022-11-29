#if PSP
#define GUGL_IMPLEMENTATION
#endif
#include <Rendering/GI.hpp>
#include <Rendering/ShaderManager.hpp>
#include <string>

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.hpp>
#include <string>
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
const std::string vert_source =
    std::string("#version 400\n") + "layout (location = 0) in vec3 aPos;\n" +
    "layout (location = 2) in vec2 aTex;\n" +
    "layout (location = 1) in vec4 aCol;\n" +
    "layout (std140) uniform Matrices { uniform mat4 proj;\n" +
    "uniform mat4 view;\n" + "uniform mat4 model;};\n" + "out vec2 uv;\n" +
    "out vec4 color;\n" + "void main() {\n" +
    "    gl_Position = proj * view * model * vec4(aPos, 1.0);\n" +
    "    uv = aTex;\n" + "    color = aCol;\n" + "}\n";
const std::string frag_source =
    std::string("#version 400\n") + "out vec4 FragColor;\n" +
    "uniform sampler2D tex;\n" + "in vec2 uv;\n" + "in vec4 color;\n" +
    "void main() {\n" + "    vec4 mc = texture(tex, uv);\n" +
    "    mc *= vec4(1.0f / 255.0f) * color;\n" + "    FragColor = mc;\n" +
    //"    FragColor.rgb = pow(mc.rgb, vec3(1.0 / 2.2));\n" +
    //"    FragColor.rgb = ((FragColor.rgb - 0.5f) * 1.5f) + 0.22f;\n" +
    "    if(FragColor.a < 0.1f)\n" + "        discard;\n" + "}\n";
const std::string frag_source2 =
    std::string("#version 400\n") + "out vec4 FragColor;\n" +
    "uniform sampler2D tex;\n" + "in vec2 uv;\n" + "in vec4 color;\n" +
    "void main() {\n" + "    vec4 mc = texture(tex, uv);\n" +
    "    mc *= vec4(1.0f / 255.0f) * color;\n" + "    FragColor = mc;\n" +
    "    FragColor.rgb = pow(mc.rgb, vec3(1.0 / 2.2));\n" +
    "    FragColor.rgb = ((FragColor.rgb - 0.5f) * 1.5f) + 0.22f;\n" +
    "    if(FragColor.a < 0.1f)\n" + "        discard;\n" + "}\n";

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

#include <Core/Application.hpp>
#include <Rendering/ShaderManager.hpp>
#include <Utilities/Assertion.hpp>

namespace GI {

#if BUILD_PC
GLFWwindow *window;
GLuint programID;
#elif BUILD_PLAT == BUILD_PSP
unsigned int __attribute__((aligned(16))) list[0x10000];
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
GLuint programID;
#endif

auto init(const RenderContextSettings app) -> void {
#if BUILD_PC
    SC_CORE_ASSERT(glfwInit(), "GLFW Init Failed!");

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(app.width, app.height, app.title, NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(0);

    SC_CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
                   "OpenGL Init Failed!");

    programID = Stardust_Celeste::Rendering::ShaderManager::get().load_shader(
        vert_source, frag_source);
    Stardust_Celeste::Rendering::ShaderManager::get().bind_shader(programID);
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

    programID = ShaderManager::get().load_shader(vert_source, frag_source);
    ShaderManager::get().bind_shader(programID);
    glUseProgram(programID);
#elif BUILD_PLAT == BUILD_3DS
    pglInit();
    pglSelectScreen(GFX_TOP, 0);
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
#endif
}

auto start_frame(bool dialog) -> void {
#if BUILD_PLAT == BUILD_PSP
    guglStartFrame(list, dialog);
#endif
}

auto end_frame(bool vsync, bool dialog) -> void {
#if BUILD_PC
    glfwPollEvents();

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
    pglSwapBuffers();
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
} // namespace GI