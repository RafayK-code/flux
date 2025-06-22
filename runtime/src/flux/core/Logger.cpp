#include <fxpch.h>

#include <flux/core/Logger.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace flux
{
    static constexpr const char* PATTERN = "[%10n] [%^%4!l%$] [%-30s:%5#] %v";

    static spdlog::level::level_enum FluxLogLevelToSpdlog(Logger::Level level);
    static spdlog::source_loc FluxSourceLocToSpdlog(const Logger::SourceLoc& loc);

    class Logger::Impl
    {
    public:
        Impl(const std::string& name)
        {
            spdlog::set_pattern(PATTERN);

            logger = spdlog::stdout_color_mt(name);
            logger->set_level(spdlog::level::info);
        }

        ~Impl() = default;

        std::shared_ptr<spdlog::logger> logger;
    };

    Logger::Logger(const std::string& name)
    {
        impl_ = new Impl(name);
    }

    Logger::~Logger()
    {
        delete impl_;
    }

    void Logger::Log(const SourceLoc& loc, Level level, const std::string& message)
    {
        impl_->logger->log(FluxSourceLocToSpdlog(loc), FluxLogLevelToSpdlog(level), message);
    }

    void Logger::Info(const SourceLoc& loc, const std::string& message)
    {
        impl_->logger->log(FluxSourceLocToSpdlog(loc), spdlog::level::info, message);
    }

    void Logger::Warn(const SourceLoc& loc, const std::string& message)
    {
        impl_->logger->log(FluxSourceLocToSpdlog(loc), spdlog::level::warn, message);
    }

    void Logger::Error(const SourceLoc& loc, const std::string& message)
    {
        impl_->logger->log(FluxSourceLocToSpdlog(loc), spdlog::level::err, message);
    }

    void Logger::Critical(const SourceLoc& loc, const std::string& message)
    {
        impl_->logger->log(FluxSourceLocToSpdlog(loc), spdlog::level::critical, message);
    }

    static spdlog::level::level_enum FluxLogLevelToSpdlog(Logger::Level level)
    {
        switch (level)
        {
        case Logger::Level::Info:       return spdlog::level::info;
        case Logger::Level::Warn:       return spdlog::level::warn;
        case Logger::Level::Error:      return spdlog::level::err;
        case Logger::Level::Critical:   return spdlog::level::critical;
        }

        return spdlog::level::info;
    }

    static spdlog::source_loc FluxSourceLocToSpdlog(const Logger::SourceLoc& loc)
    {
        return spdlog::source_loc{ loc.filename, loc.line, loc.funcname };
    }
}