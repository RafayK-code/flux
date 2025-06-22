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

    private:
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
        #define DBG_PRINT(...)      LogInfo(flux::LogManager::Ref().EngineLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_WARN(...)       LogWarn(flux::LogManager::Ref().EngineLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_ERROR(...)      LogError(flux::LogManager::Ref().EngineLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_CRIT(...)       LogCritical(flux::LogManager::Ref().EngineLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
    #else
        #define DBG_PRINT(...)      LogInfo(flux::LogManager::Ref().ClientLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_WARN(...)       LogWarn(flux::LogManager::Ref().ClientLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_ERROR(...)      LogError(flux::LogManager::Ref().ClientLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
        #define DBG_CRIT(...)       LogCritical(flux::LogManager::Ref().ClientLogger(), CREATE_NEW_SOURCE_LOC, __VA_ARGS__)
    #endif

    #define DBG_ASSERT(cond, ...)   if ((cond)) {} else { DBG_CRIT(__VA_ARGS__); DBG_BREAK; }
#else
    #define DBG_PRINT(...)
    #define DBG_WARN(...)
    #define DBG_ERROR(...)
    #define DBG_CRIT(...)

    #define DBG_ASSERT(cond, ...)
#endif