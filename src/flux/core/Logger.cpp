#include <pch.h>

#include <flux/core/Logger.h>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace flux
{
    static constexpr const char* PATTERN = "[%10n] [%^%4!l%$] [%-30s:%5#] %v";

    Logger::Logger(const std::string& clientName)
    {
        spdlog::set_pattern(PATTERN);

        logger_ = spdlog::stdout_color_mt(clientName);
        logger_->set_level(spdlog::level::info);
    }

    Logger::~Logger()
    {
    }
}