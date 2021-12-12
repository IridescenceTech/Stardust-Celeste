cmake_minimum_required(VERSION 3.17)
project(Stardust-Celeste)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)

add_library(Stardust-Celeste STATIC ${SRC_FILES} ${INC_FILES})

target_include_directories(Stardust-Celeste PUBLIC include/)
target_precompile_headers(Stardust-Celeste PRIVATE include/scpch.hpp)

enable_testing()
add_subdirectory(tests)