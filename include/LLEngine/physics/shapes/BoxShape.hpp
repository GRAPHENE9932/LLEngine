#pragma once

#include "Shape.hpp"

#include <glm/vec3.hpp>

namespace llengine {
class BoxShape : public Shape {
public:
    BoxShape(const BoxShape& box_shape) : BoxShape(box_shape.get_extents()) {}
    BoxShape(BoxShape&& box_shape) noexcept : extents(box_shape.extents), Shape(std::move(box_shape)) {}
    explicit BoxShape(const glm::vec3& extents) noexcept : extents(extents) {}

    [[nodiscard]] bool operator==(const Shape& other) const noexcept override;
    [[nodiscard]] std::shared_ptr<Shape> deep_copy() const override;

    [[nodiscard]] inline const glm::vec3& get_extents() const noexcept {
        return extents;
    }

    void scale(const glm::vec3& scale) override;

private:
    glm::vec3 extents;

    [[nodiscard]] std::unique_ptr<btCollisionShape> create_bullet_shape() const override;
};
}