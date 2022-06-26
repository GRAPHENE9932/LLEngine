#pragma once

#include <glm/vec2.hpp>

#include "../structs/Rect.hpp"

namespace utils {
    inline glm::vec2 scr_space_pos_to_gl_space(const glm::vec2 position, const glm::vec2 window_size) {
        return (position / window_size * 2.0f - 1.0f) * glm::vec2(1.0f, -1.0f);
    }

    inline glm::vec2 scr_space_scale_to_gl_space(const glm::vec2 scale, const glm::vec2 window_size) {
        return scale / window_size * 2.0f;
    }
}
