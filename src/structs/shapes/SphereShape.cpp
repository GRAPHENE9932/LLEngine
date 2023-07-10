#include "physics/shapes/SphereShape.hpp"
#include "utils/glm_bullet_conversion.hpp"

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>

using namespace llengine;

SphereShape& SphereShape::operator=(const SphereShape& other) {
    axis = get_axis();
    invalidate_shape_cache();
    return *this;
}

SphereShape& SphereShape::operator=(SphereShape&& other) noexcept {
    axis = get_axis();
    Shape::operator=(std::move(other));
    return *this;
}

bool SphereShape::operator==(const Shape& other) const noexcept {
    if (this == std::addressof(other)) {
        return true;
    }
    const SphereShape* other_ptr = dynamic_cast<const SphereShape*>(std::addressof(other));
    if (!other_ptr) {
        return false;
    }

    return this->get_axis() == other_ptr->get_axis();
}

std::shared_ptr<Shape> SphereShape::copy() const {
    return std::make_shared<SphereShape>(*this);
}

void SphereShape::scale(const glm::vec3& scale) {
    axis *= scale;

    if (cached_bullet_shape) {
        cached_bullet_shape->setLocalScaling(glm_vec3_to_bullet(scale));
    }
}

std::unique_ptr<btCollisionShape> SphereShape::create_bullet_shape() const {
    if (axis.x == axis.y && axis.y == axis.z) {
        return std::make_unique<btSphereShape>(axis.x);
    }
    else {
        float general_radius {(axis.x + axis.y + axis.z) / 3.0f};
        glm::vec3 scaling {axis / general_radius};

        auto result = std::make_unique<btSphereShape>(general_radius);
        result->setLocalScaling(glm_vec3_to_bullet(scaling));
        return result;
    }
}
