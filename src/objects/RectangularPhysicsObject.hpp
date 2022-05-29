#pragma once

#include "../structs/Cylinder.hpp"
#include "../structs/Rect.hpp"

class RectangularPhysicsObject {
public:
    Rect rect;

    virtual bool cylinder_intersects(const Cylinder& cylinder) const = 0;
};
