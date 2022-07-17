#pragma once

#include "RectangularPhysicsObject.hpp"

class RectangularWall : public RectangularPhysicsObject {
public:
    explicit RectangularWall(Rect rect);
    virtual ~RectangularWall() = default;

    bool cylinder_intersects(const Cylinder& cylinder) const override;

    glm::vec3 force_point_distance(glm::vec3 point, float distance,
                                   float* const min_distance_out = nullptr) const;

private:
    glm::vec3 force_point_distance_center_inside(glm::vec3 point, float distance) const;
    glm::vec3 force_point_distance_center_outside(glm::vec3 point, float distance) const;
};
