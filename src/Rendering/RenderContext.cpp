#include <Rendering/RenderContext.hpp>
#include <Platform.hpp>
#include <Core/Application.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#elif BUILD_PSP

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <psppower.h>
#include <psptypes.h>
#include <pspge.h>
#include <psputils.h>
#include <stdarg.h>
#include <intraFont.h>


#endif

#include <glm.hpp>

namespace Stardust_Celeste::Rendering {
#if BUILD_PC
    GLFWwindow* window;
    GLuint programID;
#elif BUILD_PLAT == BUILD_PSP
	char list[0x100000] __attribute__((aligned(64)));
	void* _fbp0;
	void* _fbp1;
	void* _zbp;
#endif

    auto to_vec4(Color& c) -> glm::vec4 {
        return {
            static_cast<float>(c.rgba.r) / 255.0f,
            static_cast<float>(c.rgba.g) / 255.0f,
            static_cast<float>(c.rgba.b) / 255.0f,
            static_cast<float>(c.rgba.a) / 255.0f
            };
    }


#if BUILD_PC
    auto compileShader(const char* source, GLenum shaderType) -> GLuint{
        auto shader = glCreateShader(shaderType);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if(!status){
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            throw std::runtime_error("Shader compile failed: " + std::string(log));
        }

        return shader;
    }

    auto linkShaders(GLuint vshader, GLuint fshader) -> GLuint{
        auto prog = glCreateProgram();
        glAttachShader(prog, vshader);
        glAttachShader(prog, fshader);

        glLinkProgram(prog);

        GLint status = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &status);

        if(!status){
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

    const std::string vert_source = std::string("#version 400\n") +
                                    "layout (location = 0) in vec3 aPos;\n" +
                                    "layout (location = 2) in vec2 aTex;\n" +
                                    "layout (location = 1) in vec4 aCol;\n" +
                                    "uniform mat4 proj;\n" +
                                    "uniform mat4 view;\n" +
                                    "uniform mat4 model;\n" +
                                    "out vec2 uv;\n" +
                                    "out vec4 color;\n" +
                                    "void main() {\n" +
                                    "    gl_Position = /* proj * view * model * */ vec4(aPos, 1.0);\n" +
                                    "    uv = aTex;\n" +
                                    "    color = aCol;\n" +
                                    "}\n";
    const std::string frag_source = std::string("#version 400\n") +
                                    "out vec4 FragColor;\n" +
                                    "uniform sampler2D tex;\n" +
                                    "in vec2 uv;\n" +
                                    "in vec4 color;\n" +
                                    "void main() {\n" +
                                    "    vec4 mc = vec4(1.0f / 255.0f);\n" +
                                    //"    mc = texture(tex, uv);\n" +
                                    "    mc *= color;\n" +
                                    "    FragColor = mc;\n" +
                                    "    if(FragColor.a == 0.0f)\n" +
                                    "        discard;\n" +
                                    "}\n";
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

        SC_CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "OpenGL Init Failed!");

        programID = loadShaders(vert_source, frag_source);
#else 

        //Get static vram buffer starts at offset 0
        //Offset 0 += memSize each time for result

        u32 offset = 0;
        _fbp0 = (void*)(offset); // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
        offset += 4 * BUF_WIDTH * SCR_HEIGHT; //PSM 8888 is 4 bytes
		_fbp1 = (void*)(offset); // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
        offset += 4 * BUF_WIDTH * SCR_HEIGHT; //PSM 8888 is 4 bytes
		_zbp = (void*)(offset); // getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_4444);

		sceGuInit();

		sceGuStart(GU_DIRECT, list);


		sceGuDrawBuffer(GU_PSM_8888, _fbp0, BUF_WIDTH);
		sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, _fbp1, BUF_WIDTH);
		sceGuDepthBuffer(_zbp, BUF_WIDTH);

		sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
		sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);

		sceGuDepthRange(50000, 10000);

		sceGuEnable(GU_SCISSOR_TEST);
		sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
		sceGuEnable(GU_SCISSOR_TEST);
		sceGuDepthFunc(GU_GEQUAL);
		sceGuEnable(GU_DEPTH_TEST);

		sceGuDisable(GU_TEXTURE_2D);
		sceGuEnable(GU_CLIP_PLANES);

		sceGuEnable(GU_CULL_FACE);
		sceGuFrontFace(GU_CCW);

		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
		sceGuAlphaFunc(GU_GREATER, 0.0f, 0xff);

		sceGuStencilFunc(GU_ALWAYS, 1, 1); // always set 1 bit in 1 bit mask
		sceGuStencilOp(GU_KEEP, GU_KEEP, GU_REPLACE); // keep value on failed test (fail and zfail) and replace on pass

		sceGuTexFilter(GU_LINEAR, GU_LINEAR);

		sceGuShadeModel(GU_SMOOTH);

		sceGuFrontFace(GU_CCW);
		sceGuEnable(GU_CULL_FACE);
		sceGuEnable(GU_BLEND);
		sceGuEnable(GU_TEXTURE_2D);

        sceGuDisable(GU_DEPTH_TEST);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumOrtho(-1, 1, -1, 1, -1, 1);
        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();

		sceGuFinish();
		sceGuSync(0, 0);

		sceDisplayWaitVblankStart();
		sceGuDisplay(GU_TRUE);

		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
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
        sceGuStart(GU_DIRECT, list); // We can only clear 1x per frame (why would you want to clear more than once?)
        sceGuClearColor(c.color);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
#endif
    }

    auto RenderContext::render() -> void {
#if BUILD_PC
        glfwPollEvents();

        if(glfwWindowShouldClose(window))
            Core::Application::Get().Exit();

        glfwSwapBuffers(window);
#else 
        sceGuFinish();
		sceGuSync(0, 0);
        sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
#endif
    }
}