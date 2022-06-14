#include "RectangularPhysicsObject.hpp"

float RectangularPhysicsObject::horizontal_distance_from_cylinder(const Cylinder& cylinder) const {
    return rect.distance_from({cylinder.position.x, cylinder.position.z}) - cylinder.radius;
}
