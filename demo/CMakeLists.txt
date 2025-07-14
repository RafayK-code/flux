cmake_minimum_required(VERSION 3.20)
project(flux-demo)

set(CMAKE_FOLDER "demo")
set(FLUX_DEMO_CPP OFF CACHE BOOL "Create a demo cpp application using the flux engine directly")
set(FLUX_DEMO_BOOTSTRAPPED OFF CACHE BOOL "Create a demo application using the flux bootstrapper and runtime")

if(${FLUX_DEMO_CPP})
    add_subdirectory(demo-cpp)
endif()

if(${FLUX_DEMO_BOOTSTRAPPED})
    add_subdirectory(demo-bootstrapped)
endif()