cmake_minimum_required(VERSION 3.20)
project("flux-bootstrapper")

set(FLUX_BOOTSTRAPPER_PUBLISH OFF CACHE BOOL "Build bootstrapper as publish")

if(FLUX_BOOTSTRAPPER_PUBLISH)
    set(DOTNET_COMMAND "publish")
    set(DOTNET_EXTRA_ARGS "--self-contained" "true" "-p:PublishSingleFile=true")
else()
    set(DOTNET_COMMAND "build")
    set(DOTNET_EXTRA_ARGS)
endif()

find_program(DOTNET_EXECUTABLE dotnet REQUIRED)

add_custom_target(${PROJECT_NAME}
    COMMAND ${DOTNET_EXECUTABLE} ${DOTNET_COMMAND}
        ${CMAKE_CURRENT_SOURCE_DIR}/Flux.Bootstrapper.CLI/Flux.Bootstrapper.CLI.csproj
        -c $<CONFIG>
        -o ${FLUX_ENGINE_OUTPUT_DIR}
        ${DOTNET_EXTRA_ARGS}
    COMMENT "Building C# Bootstrapper (${DOTNET_COMMAND} mode)"
)