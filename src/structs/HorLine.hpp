#pragma once

#include <glm/vec2.hpp>

struct HorLine {
    float lower_x, higher_x, y;

    glm::vec2 closest_point(glm::vec2 point) const;
};
