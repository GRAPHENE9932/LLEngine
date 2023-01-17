#include <glm/fwd.hpp>
#include <glm/gtx/transform.hpp>
#include <type_traits>

#include "CameraNode.hpp" // CameraNode
#include "RenderingServer.hpp" // RenderingServer
#include "nodes/core/CompleteSpatialNode.hpp"

constexpr glm::vec3 UP(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);

CameraNode::CameraNode(RenderingServer& rs, const CompleteSpatialNode::SpatialParams& p,
    float display_ratio, float fov) noexcept :
    CompleteSpatialNode(p), field_of_view(fov), aspect_ratio(display_ratio) {
    rs.register_camera_node(this);
}

glm::mat4 CameraNode::get_view_matrix() noexcept {
    if (!is_cached_view_matrix_valid) {
        recompute_view_matrix();
    }

    return cached_view_matrix;
}

glm::mat4 CameraNode::get_proj_matrix() noexcept {
    if (!is_cached_proj_matrix_valid) {
        recompute_proj_matrix();
    }

    return cached_proj_matrix;
}

void CameraNode::set_translation(const glm::vec3& new_trans) {
    CompleteSpatialNode::set_translation(new_trans);

    is_cached_view_matrix_valid = false;
}

void CameraNode::translate(const glm::vec3 &translation) {
    CompleteSpatialNode::translate(translation);

    is_cached_view_matrix_valid = false;
}

void CameraNode::set_rotation(const glm::quat &new_rotation) {
    CompleteSpatialNode::set_rotation(new_rotation);

    is_cached_view_matrix_valid = false;
}

void CameraNode::set_field_of_view(float new_field_of_view) {
    field_of_view = new_field_of_view;

    is_cached_proj_matrix_valid = false;
}

void CameraNode::set_aspect_ratio(float new_aspect_ratio) {
    aspect_ratio = new_aspect_ratio;

    is_cached_proj_matrix_valid = false;
}

void CameraNode::recompute_view_matrix() noexcept {
    const glm::vec3 forward = get_rotation() * FORWARD;
    const glm::vec3 up = get_rotation() * UP;
    const glm::vec3 position = get_translation();
    cached_view_matrix = glm::lookAt(position, position + forward, up);

    is_cached_view_matrix_valid = true;
}

void CameraNode::recompute_proj_matrix() noexcept {
    cached_proj_matrix = glm::perspective(field_of_view, aspect_ratio, 0.1f, 100.0f);

    is_cached_proj_matrix_valid = true;
}

