cmake_minimum_required(VERSION 3.17)
project(Example)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
elseif(VITA)
    add_compile_definitions(BUILD_PLAT=3)
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

if(VITA)
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

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -Wl,-q")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wl,-q")

    set(VITA_ELF_CREATE vita-elf-create)
    set(VITA_ELF_EXPORT vita-elf-export)
    set(VITA_LIBS_GEN vita-libs-gen)
    set(VITA_MAKE_FSELF vita-make-fself)
    set(VITA_MKSFOEX vita-mksfoex)
    set(VITA_PACK_VPK vita-pack-vpk)

    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

    set(VITA_APP_NAME "Stardust Test")
    set(VITA_TITLEID "SDC000001")
    set(VITA_VERSION "01.00")

    target_link_libraries(Example SceCtrl_stub vitaGL vitashark mathneon SceShaccCg_stub SceShaccCgExt SceAppMgr_stub SceGxm_stub SceCommonDialog_stub SceProcessmgr_stub SceDisplay_stub SceKernelDmacMgr_stub taihen_stub)

    vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})

    vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE sce_sys/icon0.png sce_sys/icon0.png
        FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
        FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
        FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
    )
endif()