#pragma once

#if defined(FLUX_BUILD_SHARED) && defined(FLUX_PLATFORM_WINDOWS)
    #ifdef FLUX_EXPORT
        #define FLUX_API __declspec(dllexport)
    #else
        #define FLUX_API __declspec(dllimport)
    #endif
#else
    #define FLUX_API
#endif

constexpr const char* ENGINE_NAME = "Flux";