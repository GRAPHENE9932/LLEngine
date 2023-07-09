#pragma once

#include "nodes/SpatialNode.hpp"

namespace llengine {
class RenderingServer;
class BulletPhysicsServer;

class RootNode : public SpatialNode {
public:
    RootNode(RenderingServer& rs, BulletPhysicsServer& bps) : rendering_server(rs), bullet_physics_server(bps) {}

    void set_translation(const glm::vec3& new_trans) final override {};
    void translate(const glm::vec3& translation) final override {};
    void set_scale(const glm::vec3& new_scale) final override {};
    void set_rotation(const glm::quat& new_rotation) final override {};
    void set_transform(const Transform& new_transform) final override {};

    [[nodiscard]] glm::vec3 get_translation() const noexcept final override {
        return {0.0f, 0.0f, 0.0f};
    }
    [[nodiscard]] glm::vec3 get_global_position() const noexcept final override {
        return {0.0f, 0.0f, 0.0f};
    }
    [[nodiscard]] glm::vec3 get_scale() const noexcept final override {
        return {1.0f, 1.0f, 1.0f};
    }
    [[nodiscard]] glm::vec3 get_global_scale() const noexcept final override {
        return {1.0f, 1.0f, 1.0f};
    }
    [[nodiscard]] glm::quat get_rotation() const noexcept final override {
        return glm::quat({0.0f, 0.0f, 0.0f});
    }
    [[nodiscard]] glm::quat get_global_rotation() const noexcept final override {
        return glm::quat({0.0f, 0.0f, 0.0f});
    }
    [[nodiscard]] Transform get_transform() const noexcept final override {
        return Transform();
    }
    [[nodiscard]] Transform get_global_transform() const noexcept final override {
        return Transform();
    }
    [[nodiscard]] glm::mat4 get_local_matrix() const noexcept final override {
        return Transform().calculate_matrix();
    }
    [[nodiscard]] glm::mat4 get_global_matrix() const noexcept final override {
        return Transform().calculate_matrix();
    }

    [[nodiscard]] RenderingServer& get_rendering_server() const noexcept {
        return rendering_server;
    }
    [[nodiscard]] BulletPhysicsServer& get_bullet_physics_server() const noexcept {
        return bullet_physics_server;
    }

    [[nodiscard]] bool is_attached_to_tree() const final override {
        return true;
    }
    [[nodiscard]] const RootNode& get_root_node() const final override {
        return *this;
    }
    [[nodiscard]] RootNode& get_root_node() final override {
        return *this;
    }

private:
    RenderingServer& rendering_server;
    BulletPhysicsServer& bullet_physics_server;
};
}