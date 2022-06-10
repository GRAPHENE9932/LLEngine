#pragma once

#include <glm/vec2.hpp>

#include "HorLine.hpp"
#include "../consts_and_enums.hpp"

struct VertLine {
    float lower_y, higher_y, x;

    glm::vec2 closest_point(glm::vec2 point) const;
    IntersectionCount intersection_points(const VertLine& other,
        glm::vec2& point_1, const bool include_edges = true) const;
    IntersectionCount intersection_points(const HorLine& other,
        glm::vec2& point_1, const bool include_edges = true) const;
};
