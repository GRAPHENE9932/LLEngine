#pragma once

#include <glm/vec2.hpp>

#include "../consts_and_enums.hpp"

struct Circle {
    glm::vec2 center;
    float radius;

    IntersectionCount intersection_points(const Circle& other,
                                          glm::vec2& point_1, glm::vec2& point_2) const;
};
