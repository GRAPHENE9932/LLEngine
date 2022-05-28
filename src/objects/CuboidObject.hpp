#pragma once

#include "../structs/Cylinder.hpp"
#include "../structs/Rect.hpp"

class CuboidObject {
public:
    Rect rect;
    float bottom_y, top_y;

    CuboidObject(const Rect& rect, float bottom_y, float top_y);

    inline bool cylinder_intersects(const Cylinder& cylinder) const noexcept {
        return (cylinder.lowest_y() < top_y && bottom_y < cylinder.highest_y()) &&
            rect.distance_from({cylinder.position.x, cylinder.position.z}) < cylinder.radius;
    }

    glm::vec3 push_cylinder_out(const Cylinder& cylinder, bool& is_above,
                                float* const min_distance_out = nullptr) const;
};
