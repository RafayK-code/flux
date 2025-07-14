cmake_minimum_required(VERSION 3.20)
project(demo-spirv-shaders)

set(SHADERS_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHADERS_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets/shaders")

file(GLOB SHADERS CONFIGURE_DEPENDS "${SHADERS_IN_DIR}/*.vert" "${SHADERS_IN_DIR}/*.frag")

find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/bin")

foreach(SHADER ${SHADERS})
	get_filename_component(SHADER_NAME ${SHADER} NAME)
	set(SHADER_OUT_NAME "${SHADERS_OUT_DIR}/${SHADER_NAME}.spv")
	list(APPEND SHADER_OUT_NAMES ${SHADER_OUT_NAME})
	add_custom_command(
		OUTPUT ${SHADER_OUT_NAME}
		DEPENDS ${SHADER}
		COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADERS_OUT_DIR}"
		COMMAND ${GLSLC}  ${SHADER} "-o" ${SHADER_OUT_NAME}
        VERBATIM
        )
endforeach()

add_custom_target(${PROJECT_NAME} DEPENDS ${SHADER_OUT_NAMES})