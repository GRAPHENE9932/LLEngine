#pragma once

#include "Shape.hpp"

#include <glm/vec3.hpp>

namespace llengine {
class CylinderShape : public Shape {
public:
    CylinderShape(const CylinderShape& cylinder_shape);
    CylinderShape(CylinderShape&& cylinder_shape) noexcept;
    explicit CylinderShape(float radius_x, float radius_z, float height) noexcept;
    explicit CylinderShape(const glm::vec3& semi_axis) noexcept;

    [[nodiscard]] bool operator==(const Shape& other) const noexcept override;
    [[nodiscard]] std::shared_ptr<Shape> deep_copy() const override;

    [[nodiscard]] inline glm::vec3 get_semi_axis() const noexcept {
        return semi_axis;
    }
    [[nodiscard]] inline float get_height() const noexcept {
        return get_semi_axis().y * 2.0f;
    }
    [[nodiscard]] inline float get_radius_x() const noexcept {
        return get_semi_axis().x;
    }
    [[nodiscard]] inline float get_radius_z() const noexcept {
        return get_semi_axis().z;
    }

    void scale(const glm::vec3& scale) override;

private:
    glm::vec3 semi_axis;

    [[nodiscard]] std::unique_ptr<btCollisionShape> create_bullet_shape() const override;
};
}