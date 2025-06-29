# Flux

Flux is a WIP game engine written in C++, reated both as a personal learning project and as the foundation for a fully-featured engine capable of powering real games.

> This engine is not ready for production use. APIs and architecture are subject to change.

## Goals

- Create a powerful and flexible rendering architecture, with the ability to support multiple rendering API's
- Develop an extensible asset pipeline, allowing for users to integrate and control custom preprocessing actions to assets
- Implement a scene based archicture with an ECS
- Build a UI module that can be used to create layouts effectivly

## Requirements

Flux includes most of its third-party dependencies as Git submodules, so you don’t need to install them manually. However, the following tools must be installed on your system:

- CMake version 3.20 or greater
- Vulkan SDK version 1.2 or greater
- Mono version 6.12.0 or greater
- C++ 20 compiler

## Building

The easiest way to build Flux is by using the provided Python script at `tools/build.py`. This script automatically applies common CMake arguments used by the project:

```bash
git clone --recurse-submodules https://github.com/RafayK-code/flux
cd flux
python3 tools/build.py
```

Alternatively, you can build manually using CMake:

```bash
git clone --recurse-submodules https://github.com/youruser/flux.git
cd flux
mkdir build && cd build
cmake ..
cmake --build .
```