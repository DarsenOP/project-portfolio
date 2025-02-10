//
// Created by DarsenOP on 2/9/25.
//

#ifndef LOG_H
#define LOG_H

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "../Enumerators/LogEnums.h"

class Log {
    static std::shared_ptr<spdlog::logger> logger;
public:
    // Initialize Logging System
    static void Init();

    static inline std::shared_ptr<spdlog::logger> GetLogger() { return logger; }

    static void LOG(const LogLevel& level, const std::string& message);
};

#endif //LOG_H
