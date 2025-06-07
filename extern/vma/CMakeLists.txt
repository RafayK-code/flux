cmake_minimum_required(VERSION 3.20)
project(vma)

set(VMA_SOURCES
    src/vk_mem_alloc.cpp
    include/vma/vk_mem_alloc.h
)

find_package(Vulkan REQUIRED)

add_library(${PROJECT_NAME} STATIC ${VMA_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})