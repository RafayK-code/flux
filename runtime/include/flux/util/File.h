#pragma once

#include <flux/core/base.h>

#include <filesystem>

#ifdef FLUX_PLATFORM_WINDOWS
#include <Windows.h>
#elif FLUX_PLATFORM_LINUX
#include <unistd.h>
#include <climits>
#elif FLUX_PLATFORM_MAC
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

namespace flux
{
    inline std::filesystem::path BinaryDirectory()
    {
#ifdef FLUX_PLATFORM_WINDOWS
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return std::filesystem::path(buffer).parent_path();
#elif FLUX_PLATFORM_LINUX
        char buffer[1024];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1) {
            buffer[len] = '\0';
            return std::filesystem::path(buffer).parent_path();
        }
#elif FLUX_PLATFORM_MAC
        char buffer[1024];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) == 0)
            return std::filesystem::path(buffer).parent_path();
#endif
        return "";
    }
}