#pragma once

#include <algorithm>
#include <cmath>

#include <glm/vec2.hpp>

struct Rect {
    glm::vec2 position;
    glm::vec2 size;

    Rect() = default;
    Rect(glm::vec2 position, glm::vec2 size);

    inline bool contains(const glm::vec2 point) const noexcept {
        return point.x >= position.x && point.y >= position.y &&
            point.x <= position.x + size.x && point.y <= position.y + size.y;
    }

    inline float distance_from(const glm::vec2 point) const noexcept {
        float diff_x = std::max({position.x - point.x, 0.0f, point.x - position.x - size.x});
        float diff_y = std::max({position.y - point.y, 0.0f, point.y - position.y - size.y});
        return std::sqrt(diff_x * diff_x + diff_y * diff_y);
    }
};
