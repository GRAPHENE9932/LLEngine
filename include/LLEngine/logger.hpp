#pragma once

#include <string>
#include <cstdint>
#include <string_view>

namespace llengine::logger {
    enum Severity : std::uint8_t {
        INFO = 0, WARNING = 1, ERROR = 2, FATAL_ERROR = 3
    };

    namespace internal {
        void log(Severity severity, std::string_view message) noexcept;
    }

    void enable_console_logging() noexcept;
    void disable_console_logging() noexcept;
    void set_minimum_console_log_severity(Severity severity) noexcept;

    void enable_file_logging(const std::string& path) noexcept;
    void disable_file_logging() noexcept;
    void set_minimum_file_log_severity(Severity severity) noexcept;

    template<typename T>
    void log(Severity severity, T message) {
        internal::log(severity, std::to_string(message));
    }
    template<typename T> void info(T message) { log(INFO, message); }
    template<typename T> void warning(T message) { log(WARNING, message); }
    template<typename T> void error(T message) { log(ERROR, message); }
    template<typename T> void fatal_error(T message) { log(FATAL_ERROR, message); }

    template<>
    inline void log(Severity severity, std::string_view message) {
        internal::log(severity, message);
    }
    template<>
    inline void log(Severity severity, const char* message) {
        internal::log(severity, message);
    }
    template<>
    inline void log(Severity severity, char* message) {
        internal::log(severity, message);
    }
    template<>
    inline void log(Severity severity, const std::string& message) {
        internal::log(severity, message);
    }
    template<>
    inline void log(Severity severity, std::string message) {
        internal::log(severity, message);
    }
}