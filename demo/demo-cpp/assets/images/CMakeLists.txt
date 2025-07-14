cmake_minimum_required(VERSION 3.20)
project(demo-copy-images)

set(IMAGES_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(IMAGES_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets/images")

file(GLOB IMAGES CONFIGURE_DEPENDS "${IMAGES_IN_DIR}/*.png")

foreach(IMAGE ${IMAGES})
	get_filename_component(IMAGE_NAME ${IMAGE} NAME)
	set(IMAGE_OUT_NAME "${IMAGES_OUT_DIR}/${IMAGE_NAME}")
	list(APPEND IMAGE_OUT_NAMES ${IMAGE_OUT_NAME})
	add_custom_command(
		OUTPUT ${IMAGE_OUT_NAME}
		DEPENDS ${IMAGE}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${IMAGES_OUT_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IMAGE}" "${IMAGE_OUT_NAME}"
        VERBATIM
        )
endforeach()

add_custom_target(${PROJECT_NAME} DEPENDS ${IMAGE_OUT_NAMES})