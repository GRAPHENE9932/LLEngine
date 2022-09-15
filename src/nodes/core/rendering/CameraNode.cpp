#include <glm/fwd.hpp>
#include <glm/gtx/transform.hpp>
#include <type_traits>

#include "SceneTree.hpp" // SceneTree
#include "CameraNode.hpp" // CameraNode

constexpr glm::vec3 UP(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);

CameraNode::CameraNode(const SpatialNode::SpatialParams& p, SceneTree& scene_tree) :
               SpatialNode(p, scene_tree) {
    
}

glm::mat4 CameraNode::compute_view_matrix() {
    const glm::vec3 forward = get_rotation() * FORWARD;
    const glm::vec3 up = get_rotation() * UP;
    const glm::vec3 position = get_translation();
    return glm::lookAt(position, position + forward, up);
}

void CameraNode::recompute_proj_matrix(float fov, float display_ratio) {
    proj_matrix = glm::perspective(fov, display_ratio, 0.1f, 100.0f);
}

glm::mat4 CameraNode::get_proj_matrix() const noexcept {
    return proj_matrix;
}

void CameraNode::register_myself(SpatialNode* parent) {
    scene_tree.register_node(this, parent);
}
