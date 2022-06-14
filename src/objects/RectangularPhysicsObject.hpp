#pragma once

#include "../structs/Cylinder.hpp"
#include "../structs/Rect.hpp"

class RectangularPhysicsObject {
public:
    Rect rect;

    virtual bool cylinder_intersects(const Cylinder& cylinder) const = 0;

    /// @returns Distance from rect to cylinder. Can be negative if cylinder
    /// intersects with rect.
    virtual float horizontal_distance_from_cylinder(const Cylinder& cylinder) const;
};
