cmake_minimum_required(VERSION 3.0)

project(IconHidden)

set(CMAKE_CXX_STANDARD 14)

add_executable(IconHidden wmain.cpp)

target_link_libraries(IconHidden ole32)
