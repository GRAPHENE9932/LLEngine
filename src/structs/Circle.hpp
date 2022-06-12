#pragma once

#include <glm/vec2.hpp>

struct Circle {
    glm::vec2 center;
    float radius;

    uint8_t intersection_points(const Circle& other,
                                glm::vec2& point_1, glm::vec2& point_2) const;
};
