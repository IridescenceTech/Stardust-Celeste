#include <Rendering/RenderContext.hpp>
#include <Platform.hpp>
#include <Core/Application.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#endif

#include <glm.hpp>

namespace Stardust_Celeste::Rendering {
#if BUILD_PC
    GLFWwindow* window;
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


#endif
        c.color = 0xFFFFFFFF;
        is_init = true;
    }

    auto RenderContext::terminate() -> void {
#if BUILD_PC
        glfwDestroyWindow(window);
        glfwTerminate();
#else 


#endif
    }


    auto RenderContext::clear() -> void {
#if BUILD_PC
        auto color = to_vec4(c);
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#else 


#endif
    }

    auto RenderContext::render() -> void {
#if BUILD_PC
        glfwPollEvents();

        if(glfwWindowShouldClose(window))
            Core::Application::Get().Exit();

        glfwSwapBuffers(window);
#else 


#endif
    }
}