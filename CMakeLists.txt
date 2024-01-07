cmake_minimum_required(VERSION 3.18)
project(pexeso)

set(CMAKE_CXX_STANDARD 14)

add_executable(server main.cpp)
add_executable(client1 hrac1.cpp)
add_executable(client2 hrac2.cpp)
target_link_libraries(server pthread)
