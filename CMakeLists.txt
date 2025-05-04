cmake_minimum_required(VERSION 3.20)
project(flux)

set(FLUX_BUILD_SHARED ON CACHE BOOL "Build engine as shared library")

set(CMAKE_CXX_STANDARD 17)

set(FLUX_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GLFW_PATH ${CMAKE_SOURCE_DIR}/extern/glfw)
set(GLAD_PATH ${CMAKE_SOURCE_DIR}/extern/glad)
set(SPDLOG_PATH ${CMAKE_SOURCE_DIR}/extern/spdlog)

file(GLOB_RECURSE FLUX_SRC_FILES
    "${FLUX_SRC_PATH}/*.cpp"
    "${FLUX_SRC_PATH}/*.h"
)

# handle type of library
if(${FLUX_BUILD_SHARED})
    add_library(${PROJECT_NAME} SHARED ${FLUX_SRC_FILES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_BUILD_SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FLUX_EXPORT)
else()
    add_library(${PROJECT_NAME} STATIC ${FLUX_SRC_FILES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_BUILD_STATIC)
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC ${FLUX_SRC_PATH}/pch.h)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/source_group.cmake)

# check platform
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_PLATFORM_MAC)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_PLATFORM_WINDOWS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_PLATFORM_LINUX)
endif()

# build type
target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:FLUX_DEBUG>
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Release>:FLUX_RELEASE>
)

target_include_directories(${PROJECT_NAME} PUBLIC ${FLUX_SRC_PATH})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLAD_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG_PATH}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)