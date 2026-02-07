#pragma once
#include <optional>
#include <string>

namespace logging {
enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
};

bool Init(const std::string& filename, LogLevel level);
void Log(const std::string& message,
         std::optional<LogLevel> level = std::nullopt);
void SetLogLevel(LogLevel level);
}  // namespace logging