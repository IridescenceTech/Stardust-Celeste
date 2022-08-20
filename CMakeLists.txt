cmake_minimum_required(VERSION 3.17)
project(Stardust-Celeste)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)

if(PSP)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp")
    list(APPEND SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ext/osl_sound/audio.c" "${CMAKE_CURRENT_SOURCE_DIR}/ext/osl_sound/bgm.c" "${CMAKE_CURRENT_SOURCE_DIR}/ext/osl_sound/media.c" "${CMAKE_CURRENT_SOURCE_DIR}/ext/osl_sound/pgeWav.c" "${CMAKE_CURRENT_SOURCE_DIR}/ext/osl_sound/vfsFile.c" "${CMAKE_CURRENT_SOURCE_DIR}/ext/osl_sound/VirtualFile.c")
    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES})
    target_link_libraries(Stardust-Celeste PUBLIC pspgum pspgu pspge psputility pspdisplay pspctrl pspnet pspnet_apctl psppower pspaudio)
    target_include_directories(Stardust-Celeste PUBLIC ext/)
elseif(VITA)
    list(APPEND SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ext/stb_vorbis.c}")

    SET(CMAKE_SYSTEM_NAME Generic)
    SET(CMAKE_SYSTEM_VERSION 1)
    SET(CMAKE_SYSTEM_PROCESSOR "arm7-a")
    SET(CMAKE_C_COMPILER arm-vita-eabi-gcc)
    SET(CMAKE_CXX_COMPILER arm-vita-eabi-g++)
    set(CMAKE_ASM_COMPILER arm-vita-eabi-gcc)
    set(CMAKE_STRIP arm-vita-eabi-strip)
    set(CMAKE_AR arm-vita-eabi-ar)
    set(CMAKE_LINKER arm-vita-eabi-ld)
    set(CMAKE_NM arm-vita-eabi-nm)
    set(CMAKE_OBJCOPY arm-vita-eabi-objcopy)
    set(CMAKE_OBJDUMP arm-vita-eabi-objdump)
    set(CMAKE_RANLIB arm-vita-eabi-ranlib)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVITA -g3 -Wl,-q")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVITA -g3 -Wl,-q")
    file(GLOB_RECURSE YAML_FILES ext/yaml/src/*.cpp)

    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES} ${YAML_FILES})

else()
    list(APPEND SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ext/stb_vorbis.c")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW without examples")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW without tests")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW without docs")
    add_subdirectory(ext/glfw)
    add_subdirectory(ext/openal)

    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES})

    target_include_directories(Stardust-Celeste PUBLIC ext/glfw/include/)
    target_include_directories(Stardust-Celeste PUBLIC ext/openal/include/)
    target_link_libraries(Stardust-Celeste PUBLIC glfw OpenAL)
endif()

if(!MSVC)
    target_compile_options(Stardust-Celeste PUBLIC -fno-rtti)
endif()

target_include_directories(Stardust-Celeste PUBLIC include/ ./ ext/glm/glm/)

if(NOT VITA)
    add_subdirectory(ext/yaml)
    set_property(TARGET yaml-cpp PROPERTY POSITION_INDEPENDENT_CODE OFF)
endif()

add_library(SC-Entry STATIC src/Core/Entry.cpp)
target_include_directories(SC-Entry PUBLIC include/ ext/glm/glm/)

if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
elseif(DEFINED VITA)
    add_compile_definitions(BUILD_PLAT=3)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

option(BUILD_TESTS "Build test programs" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
