cmake_minimum_required(VERSION 3.17)
project(Example)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

add_subdirectory(.. Stardust/)

add_executable(Example example.cpp)
target_include_directories(Example PUBLIC ../include/ ../ext/glm/glm/)
target_link_libraries(Example Stardust-Celeste SC-Entry)