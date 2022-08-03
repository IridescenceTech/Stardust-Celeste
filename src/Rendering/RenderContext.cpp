#include <Core/Application.hpp>
#include <Platform/Platform.hpp>
#include <Rendering/RenderContext.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.hpp>
#include <string>
#elif BUILD_PLAT == BUILD_PSP

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

#include <intraFont.h>
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
#include <Rendering/ShaderManager.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

namespace Stardust_Celeste::Rendering
{
#if BUILD_PC
    GLFWwindow *window;
    GLuint programID;
#elif BUILD_PLAT == BUILD_PSP
    char list[0x100000] __attribute__((aligned(64)));
    void *_fbp0;
    void *_fbp1;
    void *_zbp;
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
    GLuint programID;
    GLuint projLoc, viewLoc, modLoc;
#endif

    auto to_vec4(Color &c) -> glm::vec4
    {
        return {static_cast<float>(c.rgba.r) / 255.0f,
                static_cast<float>(c.rgba.g) / 255.0f,
                static_cast<float>(c.rgba.b) / 255.0f,
                static_cast<float>(c.rgba.a) / 255.0f};
    }

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

    auto RenderContext::initialize(const RenderContextSettings app) -> void
    {
#if BUILD_PC
        SC_CORE_ASSERT(glfwInit(), "GLFW Init Failed!");

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        window = glfwCreateWindow(app.width, app.height, app.title, NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        SC_CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
                       "OpenGL Init Failed!");

        programID = ShaderManager::get().load_shader(vert_source, frag_source);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#elif BUILD_PLAT == BUILD_PSP

        // Get static vram buffer starts at offset 0
        // Offset 0 += memSize each time for result

        u32 offset = 0;
        _fbp0 = (void *)(offset);             // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT,
                                              // GU_PSM_8888);
        offset += 4 * BUF_WIDTH * SCR_HEIGHT; // PSM 8888 is 4 bytes
        _fbp1 = (void *)(offset);             // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT,
                                              // GU_PSM_8888);
        offset += 4 * BUF_WIDTH * SCR_HEIGHT; // PSM 8888 is 4 bytes
        _zbp = (void *)(offset);              // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT,

        sceGuInit();

        sceGuStart(GU_DIRECT, list);

        sceGuDrawBuffer(GU_PSM_8888, _fbp0, BUF_WIDTH);
        sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, _fbp1, BUF_WIDTH);
        sceGuDepthBuffer(_zbp, BUF_WIDTH);

        sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
        sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);

        sceGuDepthRange(10000, 50000);

        sceGuEnable(GU_SCISSOR_TEST);
        sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
        sceGuEnable(GU_SCISSOR_TEST);
        sceGuDepthFunc(GU_LESS);
        sceGuEnable(GU_DEPTH_TEST);

        sceGuDisable(GU_TEXTURE_2D);
        sceGuEnable(GU_CLIP_PLANES);

        sceGuEnable(GU_CULL_FACE);
        sceGuFrontFace(GU_CCW);

        sceGuEnable(GU_BLEND);
        sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuAlphaFunc(GU_GREATER, 0x20, 0xff);

        sceGuStencilFunc(GU_ALWAYS, 1, 1); // always set 1 bit in 1 bit mask
        sceGuStencilOp(GU_KEEP, GU_KEEP,
                       GU_REPLACE); // keep value on failed test (fail and zfail)
                                    // and replace on pass

        sceGuTexFilter(GU_LINEAR, GU_LINEAR);
        sceGuShadeModel(GU_SMOOTH);
        sceGuFinish();
        sceGuSync(0, 0);

        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

        sceDisplayWaitVblankStart();
        sceGuDisplay(GU_TRUE);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumOrtho(-1, 1, -1, 1, -1, 1);
        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_VITA
        vglInit(0x800000);
        vglWaitVblankStart(GL_TRUE);

        programID = loadShaders(vert_source, frag_source);

        projLoc = glGetUniformLocation(programID, "proj");
        viewLoc = glGetUniformLocation(programID, "view");
        modLoc = glGetUniformLocation(programID, "model");

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(programID);
#endif
        c.color = 0xFFFFFFFF;
        is_init = true;
        _gfx_proj = &_gfx_ortho;
    }

    auto RenderContext::terminate() -> void
    {
#if BUILD_PC
        glfwDestroyWindow(window);
        glfwTerminate();
#elif BUILD_PLAT == BUILD_PSP
        sceGuTerm();
#elif BUILD_PLAT == BUILD_VITA
        vglEnd();
#endif
    }

    auto RenderContext::clear() -> void
    {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        auto color = to_vec4(c);
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#elif BUILD_PLAT == BUILD_PSP
        sceGuStart(GU_DIRECT, list); // We can only clear 1x per frame (why would
                                     // you want to clear more than once?)
        sceGuClearColor(c.color);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT |
                   GU_STENCIL_BUFFER_BIT);
#endif
    }

    auto RenderContext::render() -> void
    {
#if BUILD_PC
        glfwPollEvents();

        if (glfwWindowShouldClose(window))
            Core::Application::get().exit();

        glfwSwapBuffers(window);
#elif BUILD_PLAT == BUILD_PSP
        sceGuFinish();
        sceGuSync(0, 0);
        // sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
#elif BUILD_PLAT == BUILD_VITA
        vglSwapBuffers(GL_FALSE);
#endif
    }

    auto RenderContext::matrix_push() -> void
    {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _matrixStack.push_back(_gfx_model);
        _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
        sceGumPushMatrix();
#endif
    }

    auto RenderContext::matrix_pop() -> void
    {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _gfx_model = _matrixStack[_matrixStack.size() - 1];
        _matrixStack.pop_back();
#elif BUILD_PLAT == BUILD_PSP
        sceGumPopMatrix();
#endif
    }

    auto RenderContext::matrix_clear() -> void
    {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
#endif
    }

    auto RenderContext::matrix_translate(glm::vec3 v) -> void
    {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _gfx_model = glm::translate(_gfx_model, v);
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_MODEL);
        ScePspFVector3 vv = {v.x, v.y, v.z};
        sceGumTranslate(&vv);
#endif
    }

    auto RenderContext::matrix_rotate(glm::vec3 v) -> void
    {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _gfx_model = glm::rotate(_gfx_model, v.x / 180.0f * 3.14159f, {1, 0, 0});
        _gfx_model = glm::rotate(_gfx_model, v.y / 180.0f * 3.14159f, {0, 1, 0});
        _gfx_model = glm::rotate(_gfx_model, v.z / 180.0f * 3.14159f, {0, 0, 1});
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_MODEL);
        sceGumRotateX(v.x / 180.0f * 3.14159f);
        sceGumRotateY(v.y / 180.0f * 3.14159f);
        sceGumRotateZ(v.z / 180.0f * 3.14159f);
#endif
    }

    auto RenderContext::matrix_scale(glm::vec3 v) -> void
    {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _gfx_model = glm::scale(_gfx_model, v);
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_MODEL);
        ScePspFVector3 vv = {v.x, v.y, v.z};
        sceGumScale(&vv);
#endif
    }

    auto RenderContext::matrix_perspective(float fovy, float aspect, float zn,
                                           float zf) -> void
    {
        _gfx_persp = glm::perspective(fovy, aspect, zn, zf);
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _gfx_view = glm::mat4(1.0f);
        _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_PROJECTION);
        ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
        sceGumLoadMatrix(&m1);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
#endif
    }

    auto RenderContext::matrix_ortho(float l, float r, float b, float t, float zn,
                                     float zf) -> void
    {
        _gfx_ortho = glm::ortho(l, r, b, t, zn, zf);
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        _gfx_view = glm::mat4(1.0f);
        _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_PROJECTION);
        ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
        sceGumLoadMatrix(&m1);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
#endif
    }

    auto RenderContext::set_matrices() -> void
    {
#if BUILD_PC

        glm::mat4 newModel = glm::mat4(1.0f);
        for (int i = 0; i < _matrixStack.size(); i++)
        {
            newModel *= _matrixStack[i];
        }
        newModel *= _gfx_model;

        glBindBuffer(GL_UNIFORM_BUFFER, ShaderManager::get().get_current_shader().ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4),
                        glm::value_ptr(newModel));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
#elif BUILD_PLAT == BUILD_VITA

        glm::mat4 newModel = glm::mat4(1.0f);
        for (int i = 0; i < _matrixStack.size(); i++)
        {
            newModel *= _matrixStack[i];
        }
        newModel *= _gfx_model;

        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(*_gfx_proj));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(_gfx_view));
        glUniformMatrix4fv(modLoc, 1, GL_FALSE, glm::value_ptr(newModel));

#endif
    }

    auto RenderContext::matrix_view(glm::mat4 mat) -> void
    {
#if BUILD_PC
        _gfx_view = mat;

        glBindBuffer(GL_UNIFORM_BUFFER, ShaderManager::get().get_current_shader().ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                        glm::value_ptr(_gfx_view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_VIEW);
        ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(mat));
        sceGumLoadMatrix(&m1);
#elif BUILD_VITA
        _gfx_view = mat;
#endif
    }

    auto RenderContext::set_mode_2D() -> void
    {
        _gfx_proj = &_gfx_ortho;
#if BUILD_PC
        glBindBuffer(GL_UNIFORM_BUFFER, ShaderManager::get().get_current_shader().ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                        glm::value_ptr(*_gfx_proj));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        _gfx_view = glm::mat4(1.0f);

        glBindBuffer(GL_UNIFORM_BUFFER, ShaderManager::get().get_current_shader().ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                        glm::value_ptr(_gfx_view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_PROJECTION);
        ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
        sceGumLoadMatrix(&m1);
        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_VITA
        _gfx_view = glm::mat4(1.0f);
        _gfx_model = glm::mat4(1.0f);
#endif
    }
    auto RenderContext::set_mode_3D() -> void
    {
        _gfx_proj = &_gfx_persp;
#if BUILD_PC
        glBindBuffer(GL_UNIFORM_BUFFER, ShaderManager::get().get_current_shader().ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                        glm::value_ptr(*_gfx_proj));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        _gfx_view = glm::mat4(1.0f);

        glBindBuffer(GL_UNIFORM_BUFFER, ShaderManager::get().get_current_shader().ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                        glm::value_ptr(_gfx_view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
        sceGumMatrixMode(GU_PROJECTION);
        ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
        sceGumLoadMatrix(&m1);
        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_VITA
        _gfx_view = glm::mat4(1.0f);
        _gfx_model = glm::mat4(1.0f);
#endif
    }

} // namespace Stardust_Celeste::Rendering