#pragma once

#include "Rect.hpp"

struct RoundedRectangle {
    Rect rect;
    float radius;

    glm::vec2 closest_point(glm::vec2 point, float* const min_distance_out = nullptr) const;
};
