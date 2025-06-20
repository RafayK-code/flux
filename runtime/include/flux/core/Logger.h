#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <format>
#include <memory>
#include <string>

namespace flux
{
    class FLUX_API Logger
    {
    public:
        Logger(const std::string& name);
        ~Logger();

        enum class Level
        {
            Info,
            Warn,
            Error,
            Critical,
        };

        struct SourceLoc
        {
            const char* filename;
            int line;
            const char* funcname;
        };

        void Log(const SourceLoc& loc, Level level, const std::string& message);
        void Info(const SourceLoc& loc, const std::string& message);
        void Warn(const SourceLoc& loc, const std::string& message);
        void Error(const SourceLoc& loc, const std::string& message);
        void Critical(const SourceLoc& loc, const std::string& message);

    private:

        class Impl;
        Impl* impl_;
    };

    template<typename... Args>
    inline void LogInfo(Logger& logger, const Logger::SourceLoc& loc, const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            logger.Info(loc, fmt);
        }
        else
        {
            logger.Info(loc, std::vformat(fmt, std::make_format_args(args...)));
        }
    }

    template<typename... Args>
    inline void LogWarn(Logger& logger, const Logger::SourceLoc& loc, const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            logger.Warn(loc, fmt);
        }
        else
        {
            logger.Warn(loc, std::vformat(fmt, std::make_format_args(args...)));
        }
    }

    template<typename... Args>
    inline void LogError(Logger& logger, const Logger::SourceLoc& loc, const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            logger.Error(loc, fmt);
        }
        else
        {
            logger.Error(loc, std::vformat(fmt, std::make_format_args(args...)));
        }
    }

    template<typename... Args>
    inline void LogCritical(Logger& logger, const Logger::SourceLoc& loc, const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            logger.Critical(loc, fmt);
        }
        else
        {
            logger.Critical(loc, std::vformat(fmt, std::make_format_args(args...)));
        }
    }
}

/**
* Util macro to quickly create a source location (needs to be a macro)
*/
#ifndef FLUX_FUNCTION
    #define FLUX_FUNCTION  static_cast<const char *>(__FUNCTION__)
#endif

#ifndef CREATE_NEW_SOURCE_LOC
    #define CREATE_NEW_SOURCE_LOC flux::Logger::SourceLoc{ __FILE__, __LINE__, FLUX_FUNCTION }
#endif
