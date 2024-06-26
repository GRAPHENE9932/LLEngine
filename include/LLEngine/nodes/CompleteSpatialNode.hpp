#pragma once

#include "SpatialNode.hpp"
#include "Transform.hpp"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/gtc/quaternion.hpp>

#include <memory> // std::unique_ptr

namespace llengine {
class CompleteSpatialNode : public SpatialNode {
public:
    explicit CompleteSpatialNode(const Transform& transform = Transform());
    CompleteSpatialNode(const CompleteSpatialNode& other) = delete;
    CompleteSpatialNode(CompleteSpatialNode&& other) = delete;
    CompleteSpatialNode& operator=(const CompleteSpatialNode& other) = delete;
    CompleteSpatialNode& operator=(CompleteSpatialNode&& other) = delete;
    virtual ~CompleteSpatialNode();

    void set_translation(const glm::vec3& new_trans) override;
    void translate(const glm::vec3& translation) override;
    void set_scale(const glm::vec3& new_scale) override;
    void set_rotation(const glm::quat& new_rotation) override;
    void set_transform(const Transform& new_transform) override;

    glm::vec3 get_translation() const noexcept override final;
    glm::vec3 get_global_position() const noexcept override final;
    glm::vec3 get_scale() const noexcept override final;
    glm::vec3 get_global_scale() const noexcept override final;
    glm::quat get_rotation() const noexcept override final;
    glm::quat get_global_rotation() const noexcept override final;
    Transform get_transform() const noexcept override final;
    Transform get_global_transform() const noexcept override final;
    glm::mat4 get_local_matrix() const noexcept override final;
    glm::mat4 get_global_matrix() const noexcept override final;

    virtual std::unique_ptr<Node> copy() const override;

protected:
    Transform transform = Transform();
    mutable glm::mat4 cached_local_matrix = glm::mat4();
    mutable bool needs_recalculation = true;

    void recalculate_matrix() const noexcept;
};
}