#include <iomanip>
#include <iostream>
#include <sstream>
#include <variant>

#include "logger/logger.h"
#include "task_executor/task_executor.h"

namespace {
void PrintUsageInfo() {
    std::cerr << "usage: test FILE_NAME LOG_LEVEL\n\n"
              << "write a MESSAGE: msg [LOG_LEVEL] 'MESSAGE'\n"
              << "set default LOG_LEVEL: set LOG_LEVEL\n\n"
              << "LOG_LEVEL: info | warning | error\n";
}

void PrintInitError(const std::string& filepath) {
    std::cerr << "logger can not be initialized with the provided file: "
              << filepath << "\n";
}

void PrintQueryError(const std::string& line) {
    std::cerr << line << ": unknown query\n";
}

namespace logger_test {
struct SetLevelQuery {
    logging::LogLevel level;
};

struct WriteQuery {
    std::optional<logging::LogLevel> level;
    std::string message;
};

struct NoCommand {};

using ParseResult = std::variant<NoCommand, SetLevelQuery, WriteQuery>;

class QueryParser {
public:
    std::optional<logging::LogLevel> GetLogLevel(const std::string& level) {
        if (level == "info") {
            return logging::LogLevel::INFO;
        } else if (level == "warning") {
            return logging::LogLevel::WARNING;
        } else if (level == "error") {
            return logging::LogLevel::ERROR;
        }
        return std::nullopt;
    }

    ParseResult Parse(const std::string& line) {
        std::istringstream ss(line);

        std::string type;
        if (!(ss >> type)) {
            return NoCommand{};
        }

        if (type == "set") {
            auto set_command = ParseSetLevelQuery(ss);
            if (set_command.has_value()) {
                return set_command.value();
            }
        } else if (type == "msg") {
            auto msg_command = ParseWriteQuery(ss);
            if (msg_command.has_value()) {
                return msg_command.value();
            }
        }

        return NoCommand{};
    }

private:
    std::optional<SetLevelQuery> ParseSetLevelQuery(std::istringstream& ss) {
        std::string arg;

        if (ss >> arg) {
            auto log_level = GetLogLevel(arg);
            if (log_level.has_value()) {
                return std::optional<SetLevelQuery>({log_level.value()});
            }
        }

        return std::nullopt;
    }

    std::optional<WriteQuery> ParseWriteQuery(std::istringstream& ss) {
        std::string level_token;
        std::string msg_token;

        ss >> std::ws;
        if (ss.peek() != '\'') {
            if (!(ss >> level_token)) {
                return std::nullopt;
            }
        }

        if (!(ss >> std::quoted(msg_token, '\''))) {
            return std::nullopt;
        }

        auto level = GetLogLevel(level_token);

        return std::optional<WriteQuery>({level, msg_token});
    }
};
}  // namespace logger_test
}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        PrintUsageInfo();
        return -1;
    }

    Executor::TaskExecutor task_executor(1);
    logger_test::QueryParser parser;

    auto log_level = parser.GetLogLevel(argv[2]);

    if (!log_level.has_value()) {
        PrintUsageInfo();
        return -1;
    }

    bool init_success = logging::Init(argv[1], log_level.value());

    if (!init_success) {
        PrintInitError(argv[1]);
        return -1;
    }

    std::string line;
    while (std::getline(std::cin, line)) {
        auto command = parser.Parse(line);

        if (std::holds_alternative<logger_test::SetLevelQuery>(command)) {
            logger_test::SetLevelQuery set_command =
                std::get<logger_test::SetLevelQuery>(command);
            logging::SetLogLevel(set_command.level);
        } else if (std::holds_alternative<logger_test::WriteQuery>(command)) {
            logger_test::WriteQuery msg_command =
                std::get<logger_test::WriteQuery>(command);
            task_executor.Submit([msg_command]() {
                logging::Log(msg_command.message, msg_command.level);
            });
        } else {
            PrintQueryError(line);
        }
    }

    return 0;
}