# check platform
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

endif()

file(GLOB_RECURSE FLUX_PLATFORM_RENDERER_FILES
    "${FLUX_SRC_PATH}/platform/opengl/*.h"
    "${FLUX_SRC_PATH}/platform/opengl/*.cpp"
    "${FLUX_SRC_PATH}/platform/vulkan/*.h"
    "${FLUX_SRC_PATH}/platform/vulkan/*.cpp"
)

list(APPEND FLUX_SRC_FILES ${FLUX_PLATFORM_RENDERER_FILES})