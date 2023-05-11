#include "physics/shapes/BoxShape.hpp"
#include "utils/glm_bullet_conversion.hpp"

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

bool BoxShape::operator==(const Shape& other) const noexcept {
    if (this == std::addressof(other)) {
        return true;
    }
    const BoxShape* other_ptr = dynamic_cast<const BoxShape*>(std::addressof(other));
    if (!other_ptr) {
        return false;
    }

    return this->get_extents() == other_ptr->get_extents();
}

std::shared_ptr<Shape> BoxShape::deep_copy() const {
    return std::make_shared<BoxShape>(*this);
}

void BoxShape::scale(const glm::vec3& scale) {
    extents *= scale;

    if (cached_bullet_shape) {
        cached_bullet_shape->setLocalScaling(glm_vec3_to_bullet(scale));
    }
}

std::unique_ptr<btCollisionShape> BoxShape::create_bullet_shape() const {
    return std::make_unique<btBoxShape>(
        glm_vec3_to_bullet(extents / 2.0f)
    );
}
