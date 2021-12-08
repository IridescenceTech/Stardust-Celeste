cmake_minimum_required(VERSION 3.17)
project(Stardust-Test)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../Stardust-Celeste lib/)
include_directories(../Stardust-Celeste/include)

add_executable(Stardust-Test main.cpp main.cpp)
target_link_libraries(Stardust-Test Stardust-Celeste)