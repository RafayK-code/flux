# check platform
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

endif()

file(GLOB_RECURSE FLUX_PLATFORM_RENDERER_SRC_FILES
    "${FLUX_ENGINE_SRC_PATH}/platform/opengl/*.h"
    "${FLUX_ENGINE_SRC_PATH}/platform/opengl/*.cpp"

    "${FLUX_ENGINE_SRC_PATH}/platform/vulkan/*.h"
    "${FLUX_ENGINE_SRC_PATH}/platform/vulkan/*.cpp"
)

file(GLOB_RECURSE FLUX_PLATFORM_RENDERER_INC_FILES
    "${FLUX_ENGINE_INC_PATH}/platform/opengl/*.h"
    "${FLUX_ENGINE_INC_PATH}/platform/opengl/*.cpp"

    "${FLUX_ENGINE_INC_PATH}/platform/vulkan/*.h"
    "${FLUX_ENGINE_INC_PATH}/platform/vulkan/*.cpp"
)

list(APPEND FLUX_ENGINE_SRC_FILES ${FLUX_PLATFORM_RENDERER_SRC_FILES})
list(APPEND FLUX_ENGINE_INC_FILES ${FLUX_PLATFORM_RENDERER_INC_FILES})