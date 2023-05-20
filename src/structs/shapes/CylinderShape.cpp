#include "physics/shapes/CylinderShape.hpp"
#include "utils/glm_bullet_conversion.hpp"

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>

using namespace llengine;

CylinderShape::CylinderShape(const CylinderShape& cylinder_shape) :
    CylinderShape(cylinder_shape.get_semi_axis()) {}

CylinderShape::CylinderShape(CylinderShape&& cylinder_shape) noexcept :
    semi_axis(cylinder_shape.semi_axis), Shape(std::move(cylinder_shape)) {}

CylinderShape::CylinderShape(float radius_x, float radius_z, float height) noexcept :
    semi_axis(radius_x, height / 2.0f, radius_z) {}

CylinderShape::CylinderShape(const glm::vec3& semi_axis) noexcept:
    semi_axis(semi_axis) {}

bool CylinderShape::operator==(const Shape& other) const noexcept {
    if (this == std::addressof(other)) {
        return true;
    }
    const CylinderShape* other_ptr = dynamic_cast<const CylinderShape*>(std::addressof(other));
    if (!other_ptr) {
        return false;
    }

    return this->get_semi_axis() == other_ptr->get_semi_axis();
}

std::shared_ptr<Shape> CylinderShape::deep_copy() const {
    return std::make_shared<CylinderShape>(*this);
}

void CylinderShape::scale(const glm::vec3 &scale) {
    semi_axis *= scale;

    if (cached_bullet_shape) {
        cached_bullet_shape->setLocalScaling(glm_vec3_to_bullet(scale));
    }
}

std::unique_ptr<btCollisionShape> CylinderShape::create_bullet_shape() const {
    return std::make_unique<btCylinderShape>(
        glm_vec3_to_bullet(get_semi_axis())
    );
}
