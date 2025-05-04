#pragma once

#include <flux/core/base.h>
#include <flux/util/Singleton.h>

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

namespace flux
{
    /**
    * Simple wrapper over a spdlog logger
    */
    class FLUX_API Logger
    {
    public:
        Logger(const std::string& name);
        ~Logger();

        template <typename... Args>
        using FormatString = spdlog::format_string_t<Args...>;

        using SourceLoc = spdlog::source_loc;

        enum class Level
        {
            Info        = spdlog::level::info,
            Warn        = spdlog::level::warn,
            Error       = spdlog::level::err,
            Critical    = spdlog::level::critical,
        };

        inline std::shared_ptr<spdlog::logger> IntenralLogger() { return logger_; }

        template <typename... Args>
        void Log(const SourceLoc& loc, Level level, FormatString<Args...> fmt, Args&&... args)
        {
            logger_->log(loc, static_cast<spdlog::level::level_enum>(level), fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Info(const SourceLoc& loc, FormatString<Args...> fmt, Args&&... args)
        {
            logger_->log(loc, static_cast<spdlog::level::level_enum>(Level::Info), fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Warn(const SourceLoc& loc, FormatString<Args...> fmt, Args&&... args)
        {
            logger_->log(loc, static_cast<spdlog::level::level_enum>(Level::Warn), fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Error(const SourceLoc& loc, FormatString<Args...> fmt, Args&&... args)
        {
            logger_->log(loc, static_cast<spdlog::level::level_enum>(Level::Error), fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Critical(const SourceLoc& loc, FormatString<Args...> fmt, Args&&... args)
        {
            logger_->log(loc, static_cast<spdlog::level::level_enum>(Level::Critical), fmt, std::forward<Args>(args)...);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
    };
}

/**
* Util macro to quickly create a source location (needs to be a macro)
*/
#define CREATE_NEW_SOURCE_LOC flux::Logger::SourceLoc{ __FILE__, __LINE__, SPDLOG_FUNCTION }
