cmake_minimum_required(VERSION 3.18)
project(pexeso)

set(CMAKE_CXX_STANDARD 14)

add_executable(pexeso main.cpp)

target_link_libraries(pexeso pthread)
