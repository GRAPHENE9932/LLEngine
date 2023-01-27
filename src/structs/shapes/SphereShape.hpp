#pragma once

#include "Shape.hpp"

#include <utility>

class SphereShape : public Shape {
public:
    SphereShape(const SphereShape& sphere_shape) : SphereShape(sphere_shape.get_axis()) {}
    SphereShape(SphereShape&& sphere_shape) noexcept : axis(sphere_shape.axis), Shape(std::move(sphere_shape)) {}
    explicit SphereShape(const glm::vec3& axis) : axis(axis) {}
    explicit SphereShape(float radius) noexcept : axis(radius) {}

    [[nodiscard]] bool operator==(const Shape& other) const noexcept override;
    [[nodiscard]] std::shared_ptr<Shape> deep_copy() const override;

    [[nodiscard]] inline glm::vec3 get_axis() const noexcept {
        return axis;
    }

    void scale(const glm::vec3& scale) override;

private:
    glm::vec3 axis;

    [[nodiscard]] std::unique_ptr<btCollisionShape> create_bullet_shape() const override;
};
