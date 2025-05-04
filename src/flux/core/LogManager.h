#pragma once

#include <flux/core/Logger.h>
#include <flux/util/Singleton.h>

namespace flux
{
    /**
    * Singleton access point for loggers. One logger per module
    */
    class FLUX_API LogManager : public Singleton<LogManager>
    {
    public:
        ~LogManager();

        inline Logger& EngineLogger() { return engineLogger_; }
        inline Logger& ClientLogger() { return clientLogger_; }

    private:
        LogManager(const std::string& clientName);

        Logger engineLogger_;
        Logger clientLogger_;

        friend class Singleton<LogManager>;
    };
}

/**
* Macros should be used for logging over function calls. A few reasons
* 
* 1. Macros enable the usage of file and line in the correct location
* 2. Macros can be enabled/disabled depending on build config
*/

#ifdef FLUX_DEBUG
    #ifdef FLUX_ENGINE
        #define DBG_PRINT(...)      flux::LogManager::Ref().EngineLogger().Info(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_WARN(...)       flux::LogManager::Ref().EngineLogger().Warn(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_ERROR(...)      flux::LogManager::Ref().EngineLogger().Error(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_CRIT(...)       flux::LogManager::Ref().EngineLogger().Critical(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
    #else
        #define DBG_PRINT(...)      flux::LogManager::Ref().ClientLogger().Info(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_WARN(...)       flux::LogManager::Ref().ClientLogger().Warn(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_ERROR(...)      flux::LogManager::Ref().ClientLogger().Error(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_CRIT(...)       flux::LogManager::Ref().ClientLogger().Critical(CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
    #endif

    #define DBG_ASSERT(cond, ...)   if ((cond)) {} else { DBG_CRIT(__VA_ARGS__); DBG_BREAK; }
#else
    #define DBG_PRINT(...)
    #define DBG_WARN(...)
    #define DBG_ERROR(...)
    #define DBG_CRIT(...)

    #define DBG_ASSERT(cond, ...)
#endif