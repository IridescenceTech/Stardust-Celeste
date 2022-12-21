cmake_minimum_required(VERSION 3.17)
project(Stardust-Celeste)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)

option(USE_VULKAN "Use Vulkan Backend" OFF)

if(USE_VULKAN)
    if(WIN32)
        set(ENV{VULKAN_SDK} "C:\\VulkanSDK\\1.3.231.1")
    endif()

    find_package(Vulkan REQUIRED)
    add_compile_definitions(SDC_VULKAN=1)
endif()

if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
elseif(NOT VITA AND NOT 3DS AND NOT PSP)
    find_package(SDL2 REQUIRED)
    find_package(PkgConfig)
    pkg_check_modules(SDL2mixer REQUIRED SDL2_mixer)
endif()

if(PSP)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp")
    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES})
    target_link_libraries(Stardust-Celeste PUBLIC pspgum pspgu pspge psputility pspdisplay pspctrl pspnet pspnet_apctl psppower pspaudio SDL2_mixer SDL2 pspvram xmp ogg vorbisfile vorbis psphprm GL GLU glut pspvfpu)
    target_include_directories(Stardust-Celeste PUBLIC ext/ ext/gu2gl)
elseif(VITA)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp")
    include(setup_vita.cmake)

    setup_vita()
    file(GLOB_RECURSE YAML_FILES ext/yaml/src/*.cpp)

    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES} ${YAML_FILES})
    target_link_libraries(Stardust-Celeste PUBLIC SDL2_mixer SDL2 SceTouch_stub SceHid_stub vitaGL SceCtrl_stub vitashark mathneon SceShaccCg_stub SceShaccCgExt SceIme_stub SceAppMgr_stub SceGxm_stub SceCommonDialog_stub SceProcessmgr_stub SceDisplay_stub SceKernelDmacMgr_stub ScePower_stub taihen_stub SceAudioIn_stub SceAudio_stub SceMotion_stub vorbisfile vorbis ogg FLAC mikmod pthread mpg123 modplug)
elseif(3DS)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp")

    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES} ${YAML_FILES})
    target_include_directories(Stardust-Celeste PUBLIC "$ENV{DEVKITPRO}/portlibs/3ds/include" "$ENV{DEVKITPRO}/picaGL/include")
    target_link_directories(Stardust-Celeste PUBLIC "$ENV{DEVKITPRO}/picaGL/lib")
    target_link_libraries(Stardust-Celeste SDL_mixer SDL mad png vorbisidec ogg mikmod picaGL citro3d ctru)
else()
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW without examples")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW without tests")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW without docs")
    add_subdirectory(ext/glfw)

    add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES})

    target_include_directories(Stardust-Celeste PUBLIC ext/glfw/include/)
    target_link_libraries(Stardust-Celeste PUBLIC glfw)
endif()

if(WIN32)
    target_link_libraries(Stardust-Celeste PUBLIC SDL2_mixer::SDL2_mixer ${SDL2_LIBRARIES}
        ${SDL2_mixer_LIBRARIES}
        ${catkin_LIBRARIES})
elseif(NOT VITA AND NOT 3DS AND NOT PSP)
    target_link_libraries(Stardust-Celeste PUBLIC ${SDL2_LIBRARIES}
        ${SDL2mixer_LIBRARIES}
        ${catkin_LIBRARIES})
endif()

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

target_compile_options(Stardust-Celeste PUBLIC -fno-rtti)
target_include_directories(Stardust-Celeste PUBLIC include/ ext/ ext/glm/glm/)

if(NOT VITA)
    add_subdirectory(ext/yaml)
    set_property(TARGET yaml-cpp PROPERTY POSITION_INDEPENDENT_CODE OFF)
endif()

add_library(SC-Entry STATIC src/Core/Entry.cpp)
target_include_directories(SC-Entry PUBLIC include/ ext/glm/glm/)


if(USE_VULKAN)
    target_include_directories(Stardust-Celeste PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_include_directories(SC-Entry PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(Stardust-Celeste PUBLIC ${Vulkan_LIBRARIES})
endif()

if(3DS)
    target_include_directories(SC-Entry PUBLIC "$ENV{DEVKITPRO}/picaGL/include")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include_directories(/usr/local/include)
    target_link_libraries(Stardust-Celeste PUBLIC z)
endif()

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
