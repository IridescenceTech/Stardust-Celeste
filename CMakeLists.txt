cmake_minimum_required(VERSION 3.17)
project(Stardust-Celeste)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)


if(PSP)
    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES})
    target_include_directories(Stardust-Celeste PUBLIC ext/glm/glm/)
    target_link_libraries(Stardust-Celeste PUBLIC pspgum pspgu pspge pspdisplay pspctrl)
else() 
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW without examples")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW without tests")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW without docs")
    add_subdirectory(ext/glfw)

    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES} src/glad.c)

    target_include_directories(Stardust-Celeste PUBLIC ext/glfw/include/ ext/glm/glm/)
    target_link_libraries(Stardust-Celeste PUBLIC glfw)
endif()

target_include_directories(Stardust-Celeste PUBLIC include/ ./)

add_library(SC-Entry STATIC src/Core/Entry.cpp)
target_include_directories(SC-Entry PUBLIC include/)

if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

enable_testing()
add_subdirectory(tests)