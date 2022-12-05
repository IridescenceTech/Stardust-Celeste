#include <Platform/Platform.hpp>
#include <Rendering/ShaderManager.hpp>
#include <Utilities/Logger.hpp>
#include <stdexcept>
#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_3DS
#ifndef SDC_VULKAN
namespace GI {
extern GLuint programID;
}
#endif
#endif
namespace Stardust_Celeste::Rendering {
#if BUILD_PLAT != BUILD_PSP && BUILD_PLAT != BUILD_3DS
#ifndef SDC_VULKAN

ShaderManager::~ShaderManager() {
    for (auto &[key, val] : fullMap) {
        glDeleteProgram(val.programID);
        glDeleteBuffers(1, &val.ubo);
    }
    fullMap.clear();
}

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

    auto programID = linkShaders(vertShader, fragShader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    glUseProgram(programID);

    return programID;
}

auto ShaderManager::load_shader(std::string vs, std::string fs) -> int {
    auto res = loadShaders(vs, fs);

#if BUILD_PLAT != BUILD_VITA
    GLuint ubo;
    GLuint ubi = glGetUniformBlockIndex(GI::programID, "Matrices");
    glUniformBlockBinding(GI::programID, ubi, 0);
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 3 * sizeof(glm::mat4));

    auto shader = Shader{res, ubi, ubo, 0, 0, 0};
#else

    u32 projLoc = glGetUniformLocation(res, "proj");
    u32 viewLoc = glGetUniformLocation(res, "view");
    u32 modLoc = glGetUniformLocation(res, "model");

    auto shader = Shader{res, 0, 0, projLoc, viewLoc, modLoc};
#endif
    fullMap.emplace(shadCount, shader);
    return shadCount++;
}

auto ShaderManager::bind_shader(u32 id) -> void {
    if (fullMap.find(id) != fullMap.end()) {
        current_shader = id;
        auto shader = fullMap[id];
        GI::programID = shader.programID;
        glUseProgram(shader.programID);

#if BUILD_PLAT != BUILD_VITA
        glBindBuffer(GL_UNIFORM_BUFFER, shader.ubo);
#endif
    }
}
#endif
#endif
} // namespace Stardust_Celeste::Rendering