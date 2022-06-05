#pragma once

#include <glm/vec2.hpp>

struct VertLine {
    float lower_y, higher_y, x;

    glm::vec2 closest_point(glm::vec2 point) const;
};
