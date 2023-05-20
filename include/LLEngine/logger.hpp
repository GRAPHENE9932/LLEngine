#pragma once

#include <string>
#include <cstdint>

namespace llengine::logger {
    enum Severity : std::uint8_t {
        INFO = 0, WARNING = 1, ERROR = 2, FATAL_ERROR = 3
    };

    void enable_console_logging() noexcept;
    void disable_console_logging() noexcept;
    void set_minimum_console_log_severity(Severity severity) noexcept;

    void enable_file_logging(const std::string& path) noexcept;
    void disable_file_logging() noexcept;
    void set_minimum_file_log_severity(Severity severity) noexcept;

    void log(Severity severity, const std::string& message) noexcept;
    inline void info(const std::string& message) noexcept { log(INFO, message); }
    inline void warning(const std::string& message) noexcept { log(WARNING, message); }
    inline void error(const std::string& message) noexcept { log(ERROR, message); }
    inline void fatal_error(const std::string& message) noexcept { log(FATAL_ERROR, message); }
}