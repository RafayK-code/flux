file(GLOB FLUX_CORE
    "${FLUX_RUNTIME_SRC_PATH}/flux/core/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/core/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/core/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/core/*.cpp"
)

file(GLOB FLUX_CORE_CODES
    "${FLUX_RUNTIME_SRC_PATH}/flux/core/codes/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/core/codes/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/core/codes/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/core/codes/*.cpp"
)

file(GLOB FLUX_CORE_DELEGATE
    "${FLUX_RUNTIME_SRC_PATH}/flux/core/delegate/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/core/delegate/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/core/delegate/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/core/delegate/*.cpp"
)

file(GLOB FLUX_EVENTS
    "${FLUX_RUNTIME_SRC_PATH}/flux/events/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/events/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/events/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/events/*.cpp"
)

file(GLOB FLUX_UTIL
    "${FLUX_RUNTIME_SRC_PATH}/flux/util/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/util/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/util/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/util/*.cpp"
)

file(GLOB FLUX_WINDOW
    "${FLUX_RUNTIME_SRC_PATH}/flux/window/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/window/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/window/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/window/*.cpp"
)

file(GLOB FLUX_RENDERER
    "${FLUX_RUNTIME_SRC_PATH}/flux/renderer/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/renderer/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/renderer/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/renderer/*.cpp"
)

set(FLUX_GEN
    "${FLUX_RUNTIME_INC_PATH}/fxpch.h"
    "${FLUX_RUNTIME_INC_PATH}/flux.h"
)

file(GLOB FLUX_SCRIPTING
    "${FLUX_RUNTIME_SRC_PATH}/flux/scripting/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/scripting/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/scripting/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/scripting/*.cpp"
)

file(GLOB FLUX_PROJECT
    "${FLUX_RUNTIME_SRC_PATH}/flux/project/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/flux/project/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/flux/project/*.h"
    "${FLUX_RUNTIME_INC_PATH}/flux/project/*.cpp"
)

#platform specific (because of the way source group works, we dont need to check if these are actually in sources or not)

file(GLOB FLUX_OPENGL
    "${FLUX_RUNTIME_SRC_PATH}/platform/opengl/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/platform/opengl/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/platform/opengl/*.h"
    "${FLUX_RUNTIME_INC_PATH}/platform/opengl/*.cpp"
)

file(GLOB FLUX_VULKAN
    "${FLUX_RUNTIME_SRC_PATH}/platform/vulkan/*.h"
    "${FLUX_RUNTIME_SRC_PATH}/platform/vulkan/*.cpp"

    "${FLUX_RUNTIME_INC_PATH}/platform/vulkan/*.h"
    "${FLUX_RUNTIME_INC_PATH}/platform/vulkan/*.cpp"
)

source_group("gen" FILES ${FLUX_GEN})
source_group("core" FILES ${FLUX_CORE})
source_group("core/delegate" FILES ${FLUX_CORE_DELEGATE})
source_group("core/codes" FILES ${FLUX_CORE_CODES})
source_group("events" FILES ${FLUX_EVENTS})
source_group("util" FILES ${FLUX_UTIL})
source_group("window" FILES ${FLUX_WINDOW})
source_group("renderer" FILES ${FLUX_RENDERER})
source_group("scripting" FILES ${FLUX_SCRIPTING})
source_group("platform/opengl" FILES ${FLUX_OPENGL})
source_group("platform/vulkan" FILES ${FLUX_VULKAN})