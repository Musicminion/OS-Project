cmake_minimum_required(VERSION 3.22)
project(Project03 C)

set(CMAKE_C_STANDARD 11)

add_executable(Project03 main.c)

find_package(Threads REQUIRED) 
target_link_libraries(Project03 Threads::Threads)
