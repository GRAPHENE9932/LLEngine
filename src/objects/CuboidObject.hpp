#pragma once

#include "RectangularPhysicsObject.hpp"

class CuboidObject : public RectangularPhysicsObject {
public:
    enum class RelativePos : uint8_t {
        ABOVE, NEXT_TO, UNDER
    };

    float bottom_y, top_y;

    CuboidObject(const Rect& rect, float bottom_y, float top_y);

    bool cylinder_intersects(const Cylinder& cylinder) const override;

    glm::vec3 push_cylinder_out(const Cylinder& cylinder, RelativePos& pos,
            float* const min_distance_out = nullptr) const;
};
