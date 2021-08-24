cmake_minimum_required(VERSION 3.17)

project(memdrv-ctrl)

set(CMAKE_CXX_STANDARD 14)

add_library(${PROJECT_NAME} STATIC
    src/memdrv-ctrl.cpp
    )

target_include_directories(${PROJECT_NAME} PUBLIC include)