file(GLOB FLUX_CORE
    "${FLUX_SRC_PATH}/flux/core/*.h"
    "${FLUX_SRC_PATH}/flux/core/*.cpp"
)

file(GLOB FLUX_UTIL
    "${FLUX_SRC_PATH}/flux/util/*.h"
    "${FLUX_SRC_PATH}/flux/util/*.cpp"
)

set(FLUX_GEN
    "${FLUX_SRC_PATH}/pch.h"
)

source_group("gen" FILES ${FLUX_GEN})
source_group("core" FILES ${FLUX_CORE})
source_group("util" FILES ${FLUX_UTIL})