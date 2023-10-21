#include <glm/fwd.hpp>
#include <glm/gtx/transform.hpp>
#include <type_traits>

#include "nodes/SpatialNode.hpp"
#include "nodes/rendering/CameraNode.hpp" // CameraNode
#include "rendering/RenderingServer.hpp" // RenderingServer
#include "nodes/CompleteSpatialNode.hpp"

using namespace llengine;

constexpr glm::vec3 UP(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);

CameraNode::CameraNode() = default;

[[nodiscard]] const glm::mat4& CameraNode::get_view_matrix() const {
    if (!is_cached_view_matrix_valid) {
        recompute_view_matrix();
    }

    return cached_view_matrix;
}

[[nodiscard]] const glm::mat4& CameraNode::get_proj_matrix() const {
    if (get_rendering_server().get_window().get_framebuffer_size() != cached_framebuffer_size) {
        is_cached_proj_matrix_valid = false;
    }

    if (!is_cached_proj_matrix_valid) {
        recompute_proj_matrix();
    }

    return cached_proj_matrix;
}

[[nodiscard]] const glm::mat4& CameraNode::get_view_proj_matrix() const {
    if (!is_cached_view_proj_matrix_valid) {
        cached_view_proj_matrix = get_proj_matrix() * get_view_matrix();
        is_cached_view_proj_matrix_valid = true;
    }

    return cached_view_proj_matrix;
}

void CameraNode::set_translation(const glm::vec3& new_trans) {
    CompleteSpatialNode::set_translation(new_trans);

    is_cached_view_matrix_valid = false;
    is_cached_view_proj_matrix_valid = false;
}

void CameraNode::translate(const glm::vec3 &translation) {
    CompleteSpatialNode::translate(translation);

    is_cached_view_matrix_valid = false;
    is_cached_view_proj_matrix_valid = false;
}

void CameraNode::set_rotation(const glm::quat &new_rotation) {
    CompleteSpatialNode::set_rotation(new_rotation);

    is_cached_view_matrix_valid = false;
    is_cached_view_proj_matrix_valid = false;
}

void CameraNode::set_field_of_view(float new_field_of_view) {
    field_of_view = new_field_of_view;

    is_cached_proj_matrix_valid = false;
    is_cached_view_proj_matrix_valid = false;
}

[[nodiscard]] glm::vec3 CameraNode::get_direction() const {
    return get_global_rotation() * FORWARD;
}

[[nodiscard]] float CameraNode::get_far_distance() const {
    return 100.0f;
}

void CameraNode::on_attachment_to_tree_without_start() {
    SpatialNode::on_attachment_to_tree_without_start();
    get_rendering_server().register_camera_node(this);
}

void CameraNode::recompute_view_matrix() const noexcept {
    const glm::vec3 forward = get_global_rotation() * FORWARD;
    const glm::vec3 up = get_global_rotation() * UP;
    const glm::vec3 position = get_global_position();
    cached_view_matrix = glm::lookAt(position, position + forward, up);

    is_cached_view_matrix_valid = true;
}

void CameraNode::recompute_proj_matrix() const noexcept {
    const glm::ivec2 framebuffer_size = get_rendering_server().get_window().get_framebuffer_size();
    cached_proj_matrix = glm::perspective(field_of_view, static_cast<float>(framebuffer_size.x) / framebuffer_size.y, 0.1f, 100.0f);

    is_cached_proj_matrix_valid = true;
}