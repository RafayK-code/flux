cmake_minimum_required(VERSION 3.20)
project(demo-copy-meshes)

set(MESHES_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MESHES_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets/meshes")

file(GLOB MESHES CONFIGURE_DEPENDS "${MESHES_IN_DIR}/*.ply")

foreach(MESH ${MESHES})
	get_filename_component(MESH_NAME ${MESH} NAME)
	set(MESH_OUT_NAME "${MESHES_OUT_DIR}/${MESH_NAME}")
	list(APPEND MESH_OUT_NAMES ${MESH_OUT_NAME})
	add_custom_command(
		OUTPUT ${MESH_OUT_NAME}
		DEPENDS ${MESH}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MESHES_OUT_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MESH}" "${MESH_OUT_NAME}"
        VERBATIM
        )
endforeach()

add_custom_target(${PROJECT_NAME} DEPENDS ${MESH_OUT_NAMES})