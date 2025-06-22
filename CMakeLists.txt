cmake_minimum_required(VERSION 3.20)
project(flux)

find_package(Vulkan REQUIRED)

add_subdirectory(extern)

set(GLFW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw)
set(GLAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
set(SPDLOG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/spdlog)
set(VMA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/vma)
set(STB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb)
set(GLM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/glm)

add_subdirectory(runtime)