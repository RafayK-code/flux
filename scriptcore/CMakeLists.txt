cmake_minimum_required(VERSION 3.20)
project("flux-scriptcore")

find_program(DOTNET_EXECUTABLE dotnet REQUIRED)

add_custom_target(${PROJECT_NAME}
    COMMAND ${DOTNET_EXECUTABLE} "build"
        ${CMAKE_CURRENT_SOURCE_DIR}/Flux.ScriptCore/Flux.ScriptCore.csproj
        -c $<CONFIG>
        -o ${FLUX_RUNTIME_OUTPUT_DIR}
        ${DOTNET_EXTRA_ARGS}
    COMMENT "Building C# ScriptCore"
)