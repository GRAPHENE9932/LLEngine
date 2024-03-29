#include "logger.hpp"

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/ostream.h>

#include <mutex>
#include <chrono>
#include <atomic>
#include <fstream>
#include <iostream>

using namespace llengine;

std::atomic_bool console_logging_enabled {false};
std::atomic<logger::Severity> minimum_console_log_severity {logger::INFO};

std::unique_ptr<std::ofstream> log_file_stream {nullptr};
std::atomic<logger::Severity> minimum_file_log_severity {logger::WARNING};

std::mutex io_mutex;

[[nodiscard]] std::string severity_to_string(logger::Severity severity) {
    switch (severity) {
    case logger::Severity::INFO:
        return "INFO ";
    case logger::Severity::WARNING:
        return "WARNG";
    case logger::Severity::ERROR:
        return "ERROR";
    case logger::Severity::FATAL_ERROR:
        return "FATAL";
    default:
        return "UNKNW";
    }
}

void logger::enable_console_logging() noexcept {
    console_logging_enabled = true;
}

void logger::disable_console_logging() noexcept {
    console_logging_enabled = false;
}

void logger::set_minimum_console_log_severity(Severity severity) noexcept {
    minimum_console_log_severity = severity;
}

void logger::enable_file_logging(const std::string& path) noexcept {
    std::scoped_lock lock { io_mutex };

    if (log_file_stream) {
        log_file_stream->close();
        log_file_stream->open(path, std::ios::out | std::ios::app);
    }
    else {
        log_file_stream = std::make_unique<std::ofstream>(path, std::ios::out | std::ios::app);
    }
}

void logger::disable_file_logging() noexcept {
    std::scoped_lock lock { io_mutex };

    if (log_file_stream) {
        log_file_stream->close();
        log_file_stream = nullptr;
    }
}

void logger::set_minimum_file_log_severity(Severity severity) noexcept {
    minimum_file_log_severity = severity;
}

void logger::internal::log(Severity severity, std::string_view message) noexcept {
    const auto time = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());

    std::string log_string {
        fmt::format(
            "{:%Y-%m-%d %H:%M:%S} {} {}\n",
            time,
            severity_to_string(severity),
            message
        )
    };

    std::scoped_lock lock { io_mutex };

    if (console_logging_enabled) {
        std::cout << log_string;
    }

    if (log_file_stream) {
        *log_file_stream << log_string;
    }
}