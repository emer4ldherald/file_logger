#include "logger.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>

namespace {
class MetadataFormatter {
public:
    void Format(std::string& buffer, logging::LogLevel level) const {
        buffer.clear();

        auto format_time = [&buffer]() {
            constexpr size_t kMaxDataSize = std::size("[yyyy-mm-dd hh:mm:ss]");

            buffer.resize(kMaxDataSize);
            std::time_t time = std::time(nullptr);
            buffer.resize(kMaxDataSize);
            buffer.resize(std::strftime(&buffer[1], buffer.size(), "%F %T",
                                        std::localtime(&time)) +
                          1);

            buffer[0] = '[';
            buffer.append("] ");
        };

        auto format = [&buffer](const std::string& str) {
            buffer.append("[");
            buffer.append(str);
            buffer.append("] ");
        };

        auto level_to_string = [](logging::LogLevel level) {
            switch (level) {
                case logging::LogLevel::INFO: {
                    return "INFO";
                }
                case logging::LogLevel::WARNING: {
                    return "WARNING";
                }
                case logging::LogLevel::ERROR: {
                    return "ERROR";
                }
                default:
                    return "";
            }
        };

        format_time();
        format(level_to_string(level));
        buffer.append("\t: ");
    }
};
}  // namespace

namespace logging {
class FileLogger {
public:
    bool Init(const std::string& filename, LogLevel level) {
        if (file_.is_open()) {
            return false;
        }

        file_ = std::fstream(filename, std::ios::app);
        if (!file_.is_open()) {
            return false;
        }

        filename_ = filename;
        level_.store(level);
        metadata_buffer_.reserve(kBufferSize_);

        return true;
    }

    void Log(const std::string& message, std::optional<LogLevel> level) const {
        if (!level.has_value()) {
            level = level_.load();
        } else if (level < level_.load()) {
            return;
        }

        std::lock_guard guard(mtx_);

        formatter_.Format(metadata_buffer_, level.value());
        file_ << metadata_buffer_ << message << std::endl;
    }

    void SetLogLevel(LogLevel level) { level_.store(level); }

private:
    std::string filename_;
    mutable std::fstream file_;

    std::atomic<LogLevel> level_;

    MetadataFormatter formatter_;

    mutable std::mutex mtx_;
    mutable std::string metadata_buffer_;
    const size_t kBufferSize_ = 50;
};

static FileLogger global_logger;

bool Init(const std::string& filename, LogLevel level) {
    return global_logger.Init(filename, level);
}

void Log(const std::string& message, std::optional<LogLevel> level) {
    global_logger.Log(message, level);
}

void SetLogLevel(LogLevel level) { global_logger.SetLogLevel(level); }
}  // namespace logging
