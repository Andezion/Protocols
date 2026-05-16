cmake_minimum_required(VERSION 3.20)
project(signal_vizual CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

add_executable(signal_vizual signal_vizual.cpp)
target_link_libraries(signal_vizual PRIVATE raylib)
