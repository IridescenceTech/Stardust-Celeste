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

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

namespace Stardust_Celeste::Rendering {
#if BUILD_PC
GLFWwindow *window;
GLuint programID;
#elif BUILD_PLAT == BUILD_PSP
char list[0x100000] __attribute__((aligned(64)));
void *_fbp0;
void *_fbp1;
void *_zbp;
#endif

auto to_vec4(Color &c) -> glm::vec4 {
    return {static_cast<float>(c.rgba.r) / 255.0f,
            static_cast<float>(c.rgba.g) / 255.0f,
            static_cast<float>(c.rgba.b) / 255.0f,
            static_cast<float>(c.rgba.a) / 255.0f};
}

#if BUILD_PC
auto compileShader(const char *source, GLenum shaderType) -> GLuint {
    auto shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (!status) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        throw std::runtime_error("Shader compile failed: " + std::string(log));
    }

    return shader;
}

auto linkShaders(GLuint vshader, GLuint fshader) -> GLuint {
    auto prog = glCreateProgram();
    glAttachShader(prog, vshader);
    glAttachShader(prog, fshader);

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

    auto programID = linkShaders(vertShader, fragShader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    glUseProgram(programID);

    return programID;
}

const std::string vert_source =
    std::string("#version 400\n") + "layout (location = 0) in vec3 aPos;\n" +
    "layout (location = 2) in vec2 aTex;\n" +
    "layout (location = 1) in vec4 aCol;\n" + "uniform mat4 proj;\n" +
    "uniform mat4 view;\n" + "uniform mat4 model;\n" + "out vec2 uv;\n" +
    "out vec4 color;\n" + "void main() {\n" +
    "    gl_Position = proj * view * model * vec4(aPos, 1.0);\n" +
    "    uv = aTex;\n" + "    color = aCol;\n" + "}\n";
const std::string frag_source =
    std::string("#version 400\n") + "out vec4 FragColor;\n" +
    "uniform sampler2D tex;\n" + "in vec2 uv;\n" + "in vec4 color;\n" +
    "void main() {\n" + "    vec4 mc = texture(tex, uv);\n" +
    "    mc *= vec4(1.0f / 255.0f) * color;\n" + "    FragColor = mc;\n" +
    "    if(FragColor.a < 0.1f)\n" + "        discard;\n" + "}\n";
#endif

auto RenderContext::initialize(const RenderContextSettings app) -> void {
#if BUILD_PC
    SC_CORE_ASSERT(glfwInit(), "GLFW Init Failed!");

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(app.width, app.height, app.title, NULL, NULL);
    glfwMakeContextCurrent(window);

    SC_CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
                   "OpenGL Init Failed!");

    programID = loadShaders(vert_source, frag_source);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#else

    // Get static vram buffer starts at offset 0
    // Offset 0 += memSize each time for result

    u32 offset = 0;
    _fbp0 = (void *)(offset); // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT,
                              // GU_PSM_8888);
    offset += 4 * BUF_WIDTH * SCR_HEIGHT; // PSM 8888 is 4 bytes
    _fbp1 = (void *)(offset); // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT,
                              // GU_PSM_8888);
    offset += 4 * BUF_WIDTH * SCR_HEIGHT; // PSM 8888 is 4 bytes
    _zbp = (void *)(offset); // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT,
                             // GU_PSM_4444);

    sceGuInit();

    sceGuStart(GU_DIRECT, list);

    sceGuDrawBuffer(GU_PSM_8888, _fbp0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, _fbp1, BUF_WIDTH);
    sceGuDepthBuffer(_zbp, BUF_WIDTH);

    sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);

    sceGuDepthRange(0, 65535);

    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuDepthFunc(GU_LEQUAL);

    sceGuDisable(GU_TEXTURE_2D);
    sceGuEnable(GU_CLIP_PLANES);

    sceGuEnable(GU_CULL_FACE);
    sceGuFrontFace(GU_CCW);

    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

    sceGuEnable(GU_ALPHA_TEST);
    sceGuAlphaFunc(GU_GREATER, 25, 0xff);

    sceGuStencilFunc(GU_ALWAYS, 1, 1); // always set 1 bit in 1 bit mask
    sceGuStencilOp(GU_KEEP, GU_KEEP,
                   GU_REPLACE); // keep value on failed test (fail and zfail)
                                // and replace on pass

    sceGuTexFilter(GU_LINEAR, GU_LINEAR);
    sceGuShadeModel(GU_SMOOTH);

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(-1, 1, -1, 1, -1, 1);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();

#endif
    c.color = 0xFFFFFFFF;
    is_init = true;
}

auto RenderContext::terminate() -> void {
#if BUILD_PC
    glfwDestroyWindow(window);
    glfwTerminate();
#else
    sceGuTerm();
#endif
}

auto RenderContext::clear() -> void {
#if BUILD_PC
    auto color = to_vec4(c);
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#else
    sceGuStart(GU_DIRECT, list); // We can only clear 1x per frame (why would
                                 // you want to clear more than once?)
    sceGuClearColor(c.color);
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT |
               GU_STENCIL_BUFFER_BIT);
#endif
}

auto RenderContext::render() -> void {
#if BUILD_PC
    glfwPollEvents();

    if (glfwWindowShouldClose(window))
        Core::Application::get().exit();

    glfwSwapBuffers(window);
#else
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
#endif
}

auto RenderContext::matrix_push() -> void {
#if BUILD_PC
    _matrixStack.push_back(_gfx_model);
    _gfx_model = glm::mat4(1.0f);
#else
    sceGumPushMatrix();
#endif
}

auto RenderContext::matrix_pop() -> void {
#if BUILD_PC
    _gfx_model = _matrixStack[_matrixStack.size() - 1];
    _matrixStack.pop_back();
#else
    sceGumPopMatrix();
#endif
}

auto RenderContext::matrix_clear() -> void {
#if BUILD_PC
    _gfx_model = glm::mat4(1.0f);
#else
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#endif
}

auto RenderContext::matrix_translate(glm::vec3 v) -> void {
#if BUILD_PC
    _gfx_model = glm::translate(_gfx_model, v);
#else
    sceGumMatrixMode(GU_MODEL);
    ScePspFVector3 vv = {v.x, v.y, v.z};
    sceGumTranslate(&vv);
#endif
}

auto RenderContext::matrix_rotate(glm::vec3 v) -> void {
#if BUILD_PC
    _gfx_model = glm::rotate(_gfx_model, v.x, {1, 0, 0});
    _gfx_model = glm::rotate(_gfx_model, v.y, {0, 1, 0});
    _gfx_model = glm::rotate(_gfx_model, v.z, {0, 0, 1});
#else
    sceGumMatrixMode(GU_MODEL);
    sceGumRotateX(v.x / 180.0f * 3.14159f);
    sceGumRotateY(v.y / 180.0f * 3.14159f);
    sceGumRotateZ(v.z / 180.0f * 3.14159f);
#endif
}

auto RenderContext::matrix_scale(glm::vec3 v) -> void {
#if BUILD_PC
    _gfx_model = glm::scale(_gfx_model, v);
#else
    sceGumMatrixMode(GU_MODEL);
    ScePspFVector3 vv = {v.x, v.y, v.z};
    sceGumScale(&vv);
#endif
}

auto RenderContext::matrix_perspective(float fovy, float aspect, float zn,
                                       float zf) -> void {
#if BUILD_PC
    _gfx_proj = glm::perspective(fovy, aspect, zn, zf);
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#else
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(fovy, aspect, zn, zf);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#endif
}

auto RenderContext::matrix_ortho(float l, float r, float b, float t, float zn,
                                 float zf) -> void {
#if BUILD_PC
    _gfx_proj = glm::ortho(l, r, b, t, zn, zf);
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#else
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(l, r, b, t, zn, zf);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#endif
}

auto RenderContext::set_matrices() -> void {
#if BUILD_PC
    glUniformMatrix4fv(glGetUniformLocation(programID, "proj"), 1, GL_FALSE,
                       glm::value_ptr(_gfx_proj));
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE,
                       glm::value_ptr(_gfx_view));

    glm::mat4 newModel = glm::mat4(1.0f);
    for (int i = 0; i < _matrixStack.size(); i++) {
        newModel *= _matrixStack[i];
    }
    newModel *= _gfx_model;

    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE,
                       glm::value_ptr(newModel));
#endif
}

auto RenderContext::matrix_view(glm::mat4 mat) -> void {
#if BUILD_PC
    _gfx_view = mat;
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_VIEW);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(mat));
    sceGumLoadMatrix(&m1);
#endif
}

} // namespace Stardust_Celeste::Rendering