cmake_minimum_required(VERSION 3.20)
project(flux)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

get_property(FLUX_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(FLUX_IS_MULTI_CONFIG)
    set(FLUX_RUNTIME_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")
else()
    set(FLUX_RUNTIME_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

add_subdirectory(scriptcore)
add_subdirectory(runtime)
add_subdirectory(host)
add_subdirectory(bootstrapper)