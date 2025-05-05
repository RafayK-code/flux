file(GLOB FLUX_CORE
    "${FLUX_SRC_PATH}/flux/core/*.h"
    "${FLUX_SRC_PATH}/flux/core/*.cpp"
)

file(GLOB FLUX_UTIL
    "${FLUX_SRC_PATH}/flux/util/*.h"
    "${FLUX_SRC_PATH}/flux/util/*.cpp"
)

file(GLOB FLUX_WINDOW
    "${FLUX_SRC_PATH}/flux/window/*.h"
    "${FLUX_SRC_PATH}/flux/window/*.cpp"
)

file(GLOB FLUX_RENDERER
    "${FLUX_SRC_PATH}/flux/renderer/*.h"
    "${FLUX_SRC_PATH}/flux/renderer/*.cpp"
)

set(FLUX_GEN
    "${FLUX_SRC_PATH}/pch.h"
)

source_group("gen" FILES ${FLUX_GEN})
source_group("core" FILES ${FLUX_CORE})
source_group("util" FILES ${FLUX_UTIL})
source_group("window" FILES ${FLUX_WINDOW})
source_group("renderer" FILES ${FLUX_RENDERER})