#pragma once

#include <cstdint>

namespace llengine {
enum class Channel : std::uint8_t {
    NONE, RED, GREEN, BLUE, ALPHA
};

/**
 * @brief Converts channel enum to chars 'r', 'g', 'b', or 'a'
 * 
 * On invalid channel, returns 'e'.
 */
constexpr char channel_to_char(Channel channel) noexcept {
    switch (channel) {
    case Channel::RED:
        return 'r';
    case Channel::GREEN:
        return 'g';
    case Channel::BLUE:
        return 'b';
    case Channel::ALPHA:
        return 'a';
    default:
        return 'e';
    }
}
}