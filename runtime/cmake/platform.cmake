# check platform
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

endif()

file(GLOB_RECURSE FLUX_PLATFORM_RENDERER_SRC_FILES
    "${FLUX_RUNTIME_SRC_PATH}/platform/opengl/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/platform/opengl/*.cpp"

    "${FLUX_RUNTIME_SRC_PATH}/platform/vulkan/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/platform/vulkan/*.cpp"
)

file(GLOB_RECURSE FLUX_PLATFORM_RENDERER_INC_FILES
    "${FLUX_RUNTIME_INC_PATH}/platform/opengl/*.h"
    "${FLUX_RUNTIME_INC_PATH}/platform/opengl/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/platform/vulkan/*.h"
    "${FLUX_RUNTIME_INC_PATH}/platform/vulkan/*.cpp"
)

list(APPEND FLUX_RUNTIME_SRC_FILES ${FLUX_PLATFORM_RENDERER_SRC_FILES})
list(APPEND FLUX_RUNTIME_INC_FILES ${FLUX_PLATFORM_RENDERER_INC_FILES})