#pragma once

#include "../structs/Rect.hpp"
#include "../structs/Cylinder.hpp"

class RectangularWall {
public:
    Rect rect;

    RectangularWall(Rect rect);

    inline bool cylinder_intersects(const Cylinder& cylinder) const noexcept {
        return rect.distance_from({cylinder.position.x, cylinder.position.z}) < cylinder.radius;
    }

    glm::vec3 force_point_distance(glm::vec3 point, float distance,
                                   float* const min_distance_out = nullptr) const;

private:
    glm::vec3 force_point_distance_center_inside(glm::vec3 point, float distance) const;
    glm::vec3 force_point_distance_center_outside(glm::vec3 point, float distance) const;
};
