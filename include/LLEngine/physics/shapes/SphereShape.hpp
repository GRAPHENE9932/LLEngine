#pragma once

#include "Shape.hpp"

#include <utility>

namespace llengine {
class SphereShape : public Shape {
public:
    explicit SphereShape(const glm::vec3& axis) : axis(axis) {}
    explicit SphereShape(float radius) noexcept : axis(radius) {}
    SphereShape(const SphereShape& sphere_shape) : SphereShape(sphere_shape.get_axis()) {
        invalidate_shape_cache();
    }
    SphereShape(SphereShape&& sphere_shape) noexcept : axis(sphere_shape.axis), Shape(std::move(sphere_shape)) {}
    SphereShape& operator=(const SphereShape& other);
    SphereShape& operator=(SphereShape&& other) noexcept;

    [[nodiscard]] bool operator==(const Shape& other) const noexcept override;
    [[nodiscard]] std::shared_ptr<Shape> copy() const override;

    [[nodiscard]] inline glm::vec3 get_axis() const noexcept {
        return axis;
    }

    void scale(const glm::vec3& scale) override;

private:
    glm::vec3 axis;

    [[nodiscard]] std::unique_ptr<btCollisionShape> create_bullet_shape() const override;
};
}