cmake_minimum_required(VERSION 3.29)
project(rayt C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/Users/bossm/OneDrive - Temple University/Desktop/SDL2-devel-2.30.11-mingw/SDL2-2.30.11/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(rayt main.c)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

