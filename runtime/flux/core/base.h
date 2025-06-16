#pragma once

#ifdef FLUX_ENGINE
    #define FLUX_EXPORT
#endif

#if defined(FLUX_BUILD_SHARED) && defined(FLUX_PLATFORM_WINDOWS)
    #ifdef FLUX_EXPORT
        #define FLUX_API __declspec(dllexport)
    #else
        #define FLUX_API __declspec(dllimport)
    #endif
#else
    #define FLUX_API
#endif

/**
* TODO: More compiler checks
*/

#if defined(FLUX_PLATFORM_WINDOWS) && defined(_MSC_VER)
    #define FLUX_COMPILER_MSC
#endif

#ifdef FLUX_DEBUG
    #ifdef FLUX_COMPILER_MSC
        #define DBG_BREAK __debugbreak()
    #else
        #include <csignal>
        #define DBG_BREAK raise(SIGTRAP)
    #endif
#else
    #define DBG_BREAK
#endif

namespace flux
{
    constexpr const char* ENGINE_NAME = "Flux";

    constexpr int FLUX_GL_VERSION_MAJOR = 4;
    constexpr int FLUX_GL_VERSION_MINOR = 1;
}