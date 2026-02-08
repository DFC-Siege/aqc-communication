cmake_minimum_required(VERSION 3.16)

set(PICO_SDK_FETCH_FROM_GIT on)
set(PROJECT_NAME communication)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wno-psabi)

project(${PROJECT_NAME} C CXX ASM)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
