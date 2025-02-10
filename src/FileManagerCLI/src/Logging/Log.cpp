//
// Created by DarsenOP on 2/9/25.
//

#include "Log.h"

std::shared_ptr<spdlog::logger> Log::logger;

void Log::Init()
{
    logger = spdlog::stdout_color_mt("FILE_MANAGER");

    logger->set_pattern("%^[%T] [%l] %n: %v%$");
    logger->set_level(spdlog::level::trace);
}

void Log::LOG(const LogLevel& level, const std::string &message)
{
    switch (level) {
        case LogLevel::TRACE:
            logger->trace(message);
            break;
        case LogLevel::DEBUG:
            logger->debug(message);
            break;
        case LogLevel::INFO:
            logger->info(message);
            break;
        case LogLevel::WARN:
            logger->warn(message);
            break;
        case LogLevel::ERROR:
            logger->error(message);
            break;
        case LogLevel::FATAL:
            logger->critical(message);
            break;
    }
}
