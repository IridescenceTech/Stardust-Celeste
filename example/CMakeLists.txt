cmake_minimum_required(VERSION 3.17)
project(Example)

set(CMAKE_CXX_STANDARD 17)

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

add_subdirectory(.. Stardust/)

add_executable(Example example.cpp)
target_include_directories(Example PUBLIC ../include/ ../ext/glm/glm/)
target_link_libraries(Example SC-Entry Stardust-Celeste)

if(PSP)
    create_pbp_file(
        TARGET Example
        TITLE Example
        BUILD_PRX 1
    )
endif()

if(3DS)
    target_link_libraries(Example)
    include(../Tools3DS.cmake)

    add_3dsx_target(
        Example
        APP_TITLE Example
        APP_DESCRIPTION "Stardust 3DS Sample"
        APP_AUTHOR "Iridescence"
    )
endif()

if(VITA)
    include(../setup_vita.cmake)
    setup_vita()

    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

    set(VITA_APP_NAME "Stardust Test")
    set(VITA_TITLEID "SDC000001")
    set(VITA_VERSION "01.00")

    target_link_libraries(Example SceCtrl_stub vitaGL vitashark mathneon SceShaccCg_stub SceShaccCgExt SceIme_stub SceAppMgr_stub SceGxm_stub SceCommonDialog_stub SceProcessmgr_stub SceDisplay_stub SceKernelDmacMgr_stub ScePower_stub taihen_stub openal pthread SceAudioIn_stub SceAudio_stub vorbisfile vorbis ogg)

    vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})

    vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE sce_sys/icon0.png sce_sys/icon0.png
        FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
        FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
        FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
        FILE container.jpg container.jpg
    )
endif()