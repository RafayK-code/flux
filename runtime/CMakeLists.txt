cmake_minimum_required(VERSION 3.20)
project(flux-runtime)

set(FLUX_BUILD_SHARED ON CACHE BOOL "Build engine as shared library")
set(FLUX_ENABLE_THREADING ON CACHE BOOL "Support multithreading safety within flux")
set(FLUX_ENABLE_INTRUSIVE_REF OFF CACHE BOOL "Support multithreading safety within flux")
set(FLUX_PATH_TO_VULKAN_SDK "" CACHE PATH "Path to the Vulkan SDK")

find_package(Vulkan REQUIRED)

find_path(MONO_INCLUDE_DIR
    NAMES mono/jit/jit.h
    PATHS "C:/Program Files/Mono/include/mono-2.0"
)

find_library(MONO_LIBRARY
    NAMES mono-2.0-sgen mono-2.0
    PATHS "C:/Program Files/Mono/lib"
)

if(MONO_INCLUDE_DIR AND MONO_LIBRARY)
    message(STATUS "Found Mono: ${MONO_LIBRARY}")
else()
    message(FATAL_ERROR "Mono not found")
endif()

add_subdirectory(extern)

set(GLFW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw)
set(GLAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
set(SPDLOG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/spdlog)
set(VMA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/vma)
set(STB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb)
set(GLM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/glm)
set(REFLECT_CPP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/reflect-cpp)

set(FLUX_RUNTIME_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(FLUX_RUNTIME_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE FLUX_RUNTIME_SRC_FILES
    "${FLUX_RUNTIME_SRC_PATH}/flux/*.cpp"
    "${FLUX_RUNTIME_SRC_PATH}/flux/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/*.hpp"
)

file(GLOB_RECURSE FLUX_RUNTIME_INC_FILES
    "${FLUX_RUNTIME_INC_PATH}/flux/*.cpp"
    "${FLUX_RUNTIME_INC_PATH}/flux/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/*.hpp"
)

# add pch.h to sources
list(APPEND FLUX_RUNTIME_INC_FILES "${FLUX_RUNTIME_INC_PATH}/fxpch.h")
list(APPEND FLUX_RUNTIME_INC_FILES "${FLUX_RUNTIME_INC_PATH}/flux.h")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/platform.cmake)

# handle type of library
if(${FLUX_BUILD_SHARED})
    add_library(${PROJECT_NAME} SHARED ${FLUX_RUNTIME_SRC_FILES} ${FLUX_RUNTIME_INC_FILES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_BUILD_SHARED)
else()
    add_library(${PROJECT_NAME} STATIC ${FLUX_RUNTIME_SRC_FILES} ${FLUX_RUNTIME_INC_FILES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_BUILD_STATIC)
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC ${FLUX_RUNTIME_INC_PATH}/fxpch.h)

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

# other compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE FLUX_ENGINE)
if(${FLUX_ENABLE_INTRUSIVE_REF})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_USE_INTRUSIVE_REF)
endif()
if(${FLUX_ENABLE_THREADING})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_THREADSAFE)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${FLUX_RUNTIME_INC_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${FLUX_RUNTIME_SRC_PATH})

target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_PATH}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_PATH}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${SPDLOG_PATH}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${VMA_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${STB_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_PATH})
target_include_directories(${PROJECT_NAME} PUBLIC ${REFLECT_CPP_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${MONO_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE vma)
target_link_libraries(${PROJECT_NAME} PUBLIC stb)
target_link_libraries(${PROJECT_NAME} PRIVATE reflectcpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MONO_LIBRARY})

set(MONO_DLL "C:/Program Files/Mono/bin/mono-2.0-sgen.dll")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MONO_DLL}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
    COMMENT "Copying Mono runtime DLL to output directory"
)