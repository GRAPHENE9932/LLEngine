#pragma once

#include "structs/Rect.hpp"

class FloorObject {
public:
    float height;
    Rect rect;

    FloorObject(Rect rect, float height);
    bool circle_over_floor(glm::vec2 position, float radius) const;
};
