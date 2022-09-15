#pragma once

#include "structs/Rect.hpp"

class FloorObject {
public:
    Rect rect;
    float height;

    inline bool circle_over_floor(glm::vec2 position, float radius) const {
        return rect.distance_from(position) <= radius;
    }
};
