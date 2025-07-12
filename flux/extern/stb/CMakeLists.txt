cmake_minimum_required(VERSION 3.20)
project(stb)

set(STB_SOURCES
    src/stb_image.cpp
    include/stb/stb_image.h
)

add_library(${PROJECT_NAME} STATIC ${STB_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)