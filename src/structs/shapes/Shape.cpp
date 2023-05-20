#include "physics/shapes/Shape.hpp"

#include <bullet/btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

using namespace llengine;

Shape::Shape() : cached_bullet_shape(nullptr) {}

Shape::Shape(Shape&& shape) noexcept : cached_bullet_shape(shape.cached_bullet_shape.release()) {}

Shape::~Shape() = default;

btCollisionShape* Shape::get_bullet_collision_shape() {
    if (!cached_bullet_shape) {
        cached_bullet_shape = create_bullet_shape();
    }

    return cached_bullet_shape.get();
}
