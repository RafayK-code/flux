# check platform
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB_RECURSE FLUX_PLATFORM_WINDOW_FILES
        "${FLUX_SRC_PATH}/platform/glfw/*.h"
        "${FLUX_SRC_PATH}/platform/glfw/*.cpp"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB_RECURSE FLUX_PLATFORM_WINDOW_FILES
        "${FLUX_SRC_PATH}/platform/glfw/*.h"
        "${FLUX_SRC_PATH}/platform/glfw/*.cpp"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB_RECURSE FLUX_PLATFORM_WINDOW_FILES
        "${FLUX_SRC_PATH}/platform/glfw/*.h"
        "${FLUX_SRC_PATH}/platform/glfw/*.cpp"
    )
endif()

list(APPEND FLUX_SRC_FILES ${FLUX_PLATFORM_WINDOW_FILES})