cmake_minimum_required(VERSION 3.20)
project(extern)
set(CMAKE_FOLDER "extern")

add_subdirectory(glfw ${CMAKE_BINARY_DIR}/extern/glfw)
set_target_properties(glfw PROPERTIES FOLDER "extern/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "extern/GLFW3")

add_subdirectory(glad ${CMAKE_BINARY_DIR}/extern/glad)
add_subdirectory(spdlog ${CMAKE_BINARY_DIR}/extern/spdlog)
add_subdirectory(vma ${CMAKE_BINARY_DIR}/extern/vma)
add_subdirectory(stb ${CMAKE_BINARY_DIR}/extern/stb)
add_subdirectory(glm ${CMAKE_BINARY_DIR}/extern/glm)