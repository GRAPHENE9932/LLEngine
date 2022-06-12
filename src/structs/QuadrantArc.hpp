#pragma once

#include "HorLS.hpp"
#include "VertLS.hpp"
#include "Circle.hpp"

struct QuadrantArc {
    Circle circle;
    uint8_t quadrant;

    uint8_t intersection_points(const QuadrantArc& other,
                                glm::vec2& point_1, glm::vec2& point_2) const;
    uint8_t intersection_points(const HorLS& hor_line, glm::vec2& point_1) const;
    uint8_t intersection_points(const VertLS& hor_line, glm::vec2& point_1) const;
    glm::vec2 closest_point(glm::vec2 point) const;
};
