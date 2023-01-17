#pragma once

#include "SpatialNode.hpp"
#include "structs/Transform.hpp"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/gtx/quaternion.hpp> // glm::quat

#include <vector> // std::vector
#include <memory> // std::unique_ptr
#include <string> // std::string

class CompleteSpatialNode : public SpatialNode {
public:
    std::string name;

    CompleteSpatialNode(const Transform& transform);
    virtual ~CompleteSpatialNode();

    void set_translation(const glm::vec3& new_trans) override;
    void translate(const glm::vec3& translation) override;
    void set_scale(const glm::vec3& new_scale) override;
    void set_rotation(const glm::quat& new_rotation) override;

    glm::vec3 get_translation() const noexcept override;
    glm::vec3 get_global_position() const noexcept override;
    glm::vec3 get_scale() const noexcept override;
    glm::vec3 get_global_scale() const noexcept override;
    glm::quat get_rotation() const noexcept override;
    glm::quat get_global_rotation() const noexcept override;
    glm::mat4 get_local_matrix() const noexcept override;
    glm::mat4 get_global_matrix() const noexcept override;

protected:
    Transform transform;
    mutable glm::mat4 cached_local_matrix;
    mutable bool needs_recalculation = true;

    void recalculate_matrix() const noexcept;
};
