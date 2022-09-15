#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "nodes/core/SpatialNode.hpp" // SpatialNode

class SceneTree;

class CameraNode : public SpatialNode {
public:
    CameraNode(const SpatialNode::SpatialParams& p, SceneTree& scene_tree);

    glm::mat4 compute_view_matrix();
    void recompute_proj_matrix(float fov, float display_ratio);
    glm::mat4 get_proj_matrix() const noexcept;
    void register_myself(SpatialNode* parent) override;

protected:
    glm::mat4 proj_matrix;
};
