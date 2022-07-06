#pragma once

#include <glm/vec2.hpp>

#include "consts_and_enums.hpp"

/// Horizontal line segment.
struct HorLS {
    float lower_x, higher_x, y;

    glm::vec2 closest_point(glm::vec2 point) const;
    uint8_t intersection_points(const HorLS& other,
            glm::vec2& point_1, const bool include_edges = true) const;
};
