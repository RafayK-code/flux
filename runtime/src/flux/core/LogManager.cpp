#include <fxpch.h>

#include <flux/core/LogManager.h>

namespace flux
{
    LogManager::LogManager(const std::string& clientName)
        : engineLogger_(ENGINE_NAME), clientLogger_(clientName)
    {
    }

    LogManager::~LogManager()
    {
    }
}