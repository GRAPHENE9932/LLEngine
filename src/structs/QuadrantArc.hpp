#pragma once

#include "HorLine.hpp"
#include "VertLine.hpp"
#include "Circle.hpp"

struct QuadrantArc {
    Circle circle;
    uint8_t quadrant;

    IntersectionCount intersection_points(const QuadrantArc& other,
                                          glm::vec2& point_1, glm::vec2& point_2) const;
    IntersectionCount intersection_points(const HorLine& hor_line, glm::vec2& point_1) const;
    IntersectionCount intersection_points(const VertLine& hor_line, glm::vec2& point_1) const;
    glm::vec2 closest_point(glm::vec2 point) const;
};
