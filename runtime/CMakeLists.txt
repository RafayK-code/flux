cmake_minimum_required(VERSION 3.20)
project(flux-runtime)

set(CMAKE_CXX_STANDARD 20)
set(GAME_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE GAME_SRC_FILES
    "${GAME_SRC_PATH}/*.cpp"
    "${GAME_SRC_PATH}/*.h"
)

add_executable(${PROJECT_NAME} ${GAME_SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${GAME_SRC_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE flux)

add_dependencies(flux-runtime flux-scriptcore)