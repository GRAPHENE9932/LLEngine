#pragma once

#include <glm/vec2.hpp>

#include "HorLS.hpp"
#include "consts_and_enums.hpp"

/// Vertical line segment.
struct VertLS {
    float lower_y, higher_y, x;

    glm::vec2 closest_point(glm::vec2 point) const;
    uint8_t intersection_points(const VertLS& other,
            glm::vec2& point_1, const bool include_edges = true) const;
    uint8_t intersection_points(const HorLS& other,
            glm::vec2& point_1, const bool include_edges = true) const;
};
