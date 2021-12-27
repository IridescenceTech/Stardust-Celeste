#include <Rendering/RenderContext.hpp>
#include <Platform.hpp>
#include <Core/Application.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
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
		sceGuFinish();
		sceGuSync(0, 0);

		sceDisplayWaitVblankStart();
		sceGuDisplay(GU_TRUE);

		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
		sceGuShadeModel(GU_SMOOTH);

		sceGuFrontFace(GU_CCW);
		sceGuEnable(GU_CULL_FACE);
		sceGuEnable(GU_BLEND);
		sceGuEnable(GU_TEXTURE_2D);
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