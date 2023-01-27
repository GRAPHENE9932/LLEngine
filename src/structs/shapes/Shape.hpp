#pragma once

#include <glm/vec3.hpp>

#include <memory>

class btCollisionShape;

class Shape {
public:
    Shape();
    Shape(const Shape& shape) = delete;
    Shape(Shape&& shape) noexcept;
    virtual ~Shape();

    virtual bool operator==(const Shape& other) const noexcept = 0;

    /**
     * @brief Returns non-owning pointer to the btCollisionShape.
     *
     * If called multiple times, returns the same pointer to
     * the same object.
     */
    btCollisionShape* get_bullet_collision_shape();

    [[nodiscard]] virtual std::shared_ptr<Shape> deep_copy() const = 0;

    /**
     * @brief Scales the shape in-place.
     */
    virtual void scale(const glm::vec3& scale) = 0;

protected:
    std::unique_ptr<btCollisionShape> cached_bullet_shape;

    [[nodiscard]] virtual std::unique_ptr<btCollisionShape> create_bullet_shape() const = 0;
};
