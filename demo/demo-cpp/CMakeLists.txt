cmake_minimum_required(VERSION 3.20)
project(flux-demo-cpp)

add_subdirectory(assets)

set(CMAKE_CXX_STANDARD 20)
set(DEMO_CPP_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE DEMO_CPP_SRC_FILES
    "${DEMO_CPP_SRC_PATH}/*.cpp"
    "${DEMO_CPP_SRC_PATH}/*.h"
)

add_executable(${PROJECT_NAME} ${DEMO_CPP_SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${DEMO_CPP_SRC_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE flux)

add_dependencies(${PROJECT_NAME} demo-spirv-shaders)
add_dependencies(${PROJECT_NAME} demo-copy-images)
add_dependencies(${PROJECT_NAME} demo-copy-meshes)