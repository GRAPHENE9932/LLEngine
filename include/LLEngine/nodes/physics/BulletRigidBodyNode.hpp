#pragma once

#include "Transform.hpp"
#include "nodes/SpatialNode.hpp"

#include <glm/vec3.hpp>

#include <memory>

class btRigidBody;
class btCollisionShape;

namespace llengine {
class Shape;
class BulletPhysicsServer;

class BulletRigidBodyNode : public SpatialNode {
public:
    BulletRigidBodyNode();
    BulletRigidBodyNode(const BulletRigidBodyNode& node) = delete;
    BulletRigidBodyNode(BulletRigidBodyNode&& node) = delete;
    BulletRigidBodyNode& operator=(const BulletRigidBodyNode& node) = delete;
    BulletRigidBodyNode& operator=(BulletRigidBodyNode&& node) = delete;
    ~BulletRigidBodyNode() noexcept override;

    void update() override {
        SpatialNode::update();
    }

    /**
     * @brief This method must be called by the physics
     * server every time it's about to do a physics step.
     */
    void before_simulation_step();

    void set_translation(const glm::vec3& new_trans) override;
    void translate(const glm::vec3& translation) override;
    void set_scale(const glm::vec3& new_scale) override;
    void set_rotation(const glm::quat& new_rotation) override;
    void set_transform(const Transform& new_transform) override;

    void set_global_scale(const glm::vec3& new_scale);
    void set_global_transform(const Transform& new_transform);

    [[nodiscard]] glm::vec3 get_translation() const noexcept final;
    [[nodiscard]] glm::vec3 get_global_position() const noexcept final;
    [[nodiscard]] glm::vec3 get_scale() const noexcept final;
    [[nodiscard]] glm::vec3 get_global_scale() const noexcept final;
    [[nodiscard]] glm::quat get_rotation() const noexcept final;
    [[nodiscard]] glm::quat get_global_rotation() const noexcept final;
    [[nodiscard]] Transform get_transform() const noexcept final;
    [[nodiscard]] Transform get_global_transform() const noexcept final;
    [[nodiscard]] glm::mat4 get_local_matrix() const noexcept final;
    [[nodiscard]] glm::mat4 get_global_matrix() const noexcept final;

    /**
     * @brief returns non-owning pointer to the btRigidBody object.
     */
    [[nodiscard]] inline btRigidBody* get_bt_rigid_body() const noexcept {
        return bt_rigid_body.get();
    }

    void set_shape(const std::shared_ptr<Shape>& shape);
    /**
     * @brief Set mass of the rigid body. If mass is in
     * range (0, +infinity) then it will be dynamic. If
     * mass equals to 0, then it will be static.
     * Otherwise, an error will be thrown.
     *
     * @throws std::invalid_argument if mass value is invalid.
     */
    void set_mass(float new_mass);
    [[nodiscard]] float get_mass() const noexcept;

    void set_shape(const Shape& shape);
    void create_bullet_body(const Shape& new_shape, const Transform& transform, float new_mass);

    void on_attachment_to_tree() override;

private:
    Transform previous_parent_transform;

    std::shared_ptr<Shape> shape;

    std::unique_ptr<btRigidBody> bt_rigid_body;

    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    float cached_set_mass = 0.0f;
    glm::quat cached_set_rotation = glm::quat({0.0f, 0.0f, 0.0f});
    glm::vec3 cached_set_translation = {0.0f, 0.0f, 0.0f};
};
}