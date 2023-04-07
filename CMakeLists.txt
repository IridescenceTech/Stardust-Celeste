cmake_minimum_required(VERSION 3.17)
project(Stardust-Celeste)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)

add_compile_definitions(MAKE_LIB)
set(LUA_LIBRARY_SRC ext/lua/onelua.c)

# Whether or not to use Vulkan / Direct X Mode
option(EXPERIMENTAL_GRAPHICS "Use Vulkan and DirectX Mode" OFF)

# Vulkan Include
if(NOT PSP AND NOT 3DS AND NOT VITA)
    if(EXPERIMENTAL_GRAPHICS)
        if(WIN32)
            set(Vulkan_LIBRARY "C:\\VulkanSDK\\1.3.231.1\\Lib\\vulkan-1.lib")
        endif()
        find_package(Vulkan REQUIRED)
    endif()
endif()

# SDL 2 Requirement
if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
elseif(NOT VITA AND NOT 3DS AND NOT PSP)
    find_package(SDL2 REQUIRED)
    find_package(PkgConfig)
    pkg_check_modules(SDL2mixer REQUIRED SDL2_mixer)
endif()

# ZLib Requirement
add_subdirectory(ext/zlib)

if(PSP)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp")
    add_library(Stardust-Celeste STATIC ${LUA_LIBRARY_SRC} ${SRC_FILES} ${INC_FILES})
    target_link_libraries(Stardust-Celeste PUBLIC pspgum pspgu pspge psputility pspdisplay pspctrl pspnet pspnet_apctl psppower pspaudio SDL2_mixer SDL2 pspvram xmp ogg vorbisfile vorbis psphprm GL GLU glut pspvfpu zlibstatic)
    target_include_directories(Stardust-Celeste PUBLIC ext/ ext/gu2gl ext/zlib)
elseif(VITA)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp")
    include(setup_vita.cmake)

    setup_vita()
    file(GLOB_RECURSE YAML_FILES ext/yaml/src/*.cpp)

    add_library(Stardust-Celeste STATIC  ${LUA_LIBRARY_SRC} ${SRC_FILES} ${INC_FILES} ${YAML_FILES})
    target_link_libraries(Stardust-Celeste PUBLIC SDL2_mixer SDL2 SceTouch_stub SceHid_stub vitaGL SceCtrl_stub vitashark mathneon SceShaccCg_stub SceShaccCgExt SceIme_stub SceAppMgr_stub SceGxm_stub SceCommonDialog_stub SceProcessmgr_stub SceDisplay_stub SceKernelDmacMgr_stub ScePower_stub taihen_stub SceAudioIn_stub SceAudio_stub SceMotion_stub vorbisfile vorbis ogg FLAC mikmod pthread mpg123 modplug zlibstatic)
elseif(3DS)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp")

    add_library(Stardust-Celeste STATIC ${LUA_LIBRARY_SRC} ${SRC_FILES} ${INC_FILES} ${YAML_FILES})
    target_include_directories(Stardust-Celeste PUBLIC "$ENV{DEVKITPRO}/portlibs/3ds/include" "$ENV{DEVKITPRO}/picaGL/include" ext/zlib)
    target_link_directories(Stardust-Celeste PUBLIC "$ENV{DEVKITPRO}/picaGL/lib")
    target_link_libraries(Stardust-Celeste SDL_mixer SDL mad png vorbisidec ogg mikmod picaGL citro3d ctru zlibstatic)
else()
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW without examples")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW without tests")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW without docs")
    add_subdirectory(ext/glfw)

    add_library(Stardust-Celeste STATIC  ${LUA_LIBRARY_SRC} ${SRC_FILES} ${INC_FILES})

    target_include_directories(Stardust-Celeste PUBLIC ext/glfw/include/ ext/zlib)
    target_link_libraries(Stardust-Celeste PUBLIC glfw zlib)
endif()

# No experimental graphics define
if(NOT EXPERIMENTAL_GRAPHICS)
    target_compile_definitions(Stardust-Celeste PUBLIC NO_EXPERIMENTAL_GRAPHICS)
endif()


# SDL2 for WIN32/POSIX
if(WIN32)
    target_link_libraries(Stardust-Celeste PUBLIC ws2_32 SDL2_mixer::SDL2_mixer ${SDL2_LIBRARIES}
        ${SDL2_mixer_LIBRARIES}
        ${catkin_LIBRARIES})
elseif(NOT VITA AND NOT 3DS AND NOT PSP)
    target_link_libraries(Stardust-Celeste PUBLIC ${SDL2_LIBRARIES}
        ${SDL2mixer_LIBRARIES}
        ${catkin_LIBRARIES})
endif()

# Use EASTL
option(USE_EASTL "Use EA Standard Template Library" OFF)
if(USE_EASTL)
    include_directories(${EASTL_ROOT_DIR}/include)
    include_directories(${EASTL_ROOT_DIR}/test/packages/EAAssert/include)
    include_directories(${EASTL_ROOT_DIR}/test/packages/EABase/include/Common)
    include_directories(${EASTL_ROOT_DIR}/test/packages/EAMain/include)
    include_directories(${EASTL_ROOT_DIR}/test/packages/EAStdC/include)
    include_directories(${EASTL_ROOT_DIR}/test/packages/EATest/include)
    include_directories(${EASTL_ROOT_DIR}/test/packages/EAThread/include)
    target_link_directories(Stardust-Celeste PUBLIC ${EASTL_ROOT_DIR}/build)
    add_custom_target(NatVis SOURCES ${EASTL_ROOT_DIR}/doc/EASTL.natvis)
    target_link_libraries(Stardust-Celeste PUBLIC EASTL)
    add_compile_definitions(USE_EASTL=1)
endif()

if(PSP)
    target_compile_options(Stardust-Celeste PUBLIC -fno-rtti)
endif()

# Includes
target_include_directories(Stardust-Celeste PUBLIC include/ ext/ ext/assimp/include ext/lua)

if(NOT VITA)
    add_subdirectory(ext/yaml)
    set_property(TARGET yaml-cpp PROPERTY POSITION_INDEPENDENT_CODE OFF)
endif()

# Assimp
#set(BUILD_SHARED_LIBS OFF)
#add_subdirectory(ext/assimp)

# SC-Entry Library Artifact
add_library(SC-Entry STATIC src/Core/Entry.cpp)
# SC-Entry Include
target_include_directories(SC-Entry PUBLIC include/)
if(NOT EXPERIMENTAL_GRAPHICS)
    target_compile_definitions(SC-Entry PUBLIC NO_EXPERIMENTAL_GRAPHICS)
endif()


# Lua Interpreter
add_executable(SC-Interpreter interpreter/interpreter.cpp)
target_include_directories(SC-Interpreter PUBLIC ../include/)
target_link_libraries(SC-Interpreter SC-Entry Stardust-Celeste)
if(WIN32)
    target_link_libraries(SC-Interpreter ws2_32 stdc++)
endif()

# Vulkan
if(EXPERIMENTAL_GRAPHICS)
    if(NOT PSP AND NOT 3DS AND NOT VITA)
        target_include_directories(Stardust-Celeste PUBLIC ${Vulkan_INCLUDE_DIRS})
        target_include_directories(SC-Entry PUBLIC ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(Stardust-Celeste PUBLIC Vulkan::Vulkan)
    endif()
endif()

if(3DS)
    target_include_directories(SC-Entry PUBLIC "$ENV{DEVKITPRO}/picaGL/include")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include_directories(/usr/local/include)
    target_link_libraries(Stardust-Celeste PUBLIC z)
endif()

# Define Platform Build
# TODO: Make into CMake Include
if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
elseif(VITA)
    add_compile_definitions(BUILD_PLAT=3)
elseif(3DS)
    add_compile_definitions(BUILD_PLAT=7)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()
