#pragma once

#include <string> // std::string
#include <sstream> // std::stringstream
#include <stdexcept> // std::runtime_error
#include <string_view> // std::string_view

/**
 * @brief Takes operation message and what is uninitialized.
 *
 * what() will return message in format
 * "<operation message> <what is uninitialized> is uninitialized."
 */
class uninitialized_error : public std::runtime_error {
public:
    uninitialized_error(std::string_view operation_message,
                        std::string_view what_is_uninitialized) :
        std::runtime_error(make_message(operation_message, what_is_uninitialized)) {}

private:
    std::string message;

    static std::string make_message(
        std::string_view operation_message,
        std::string_view what_is_uninitialized) noexcept {

        std::stringstream ss;
        ss << operation_message << ' ' << what_is_uninitialized <<
            " is uninitialized.";
        return ss.str();
    }
};
