#include <glm/fwd.hpp>

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
    float current_aspect_ratio = get_aspect_ratio();
    if (current_aspect_ratio != previous_aspect_ratio) {
        invalidate_proj_matrix_cache();
    }
    previous_aspect_ratio = current_aspect_ratio;

    if (!is_cached_proj_matrix_valid) {
        recompute_proj_matrix();
    }

    recompute_proj_matrix();
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

    invalidate_view_matrix_cache();
}

void CameraNode::translate(const glm::vec3 &translation) {
    CompleteSpatialNode::translate(translation);

    invalidate_view_matrix_cache();
}

void CameraNode::set_rotation(const glm::quat &new_rotation) {
    CompleteSpatialNode::set_rotation(new_rotation);

    invalidate_view_matrix_cache();
}

void CameraNode::set_field_of_view(float new_field_of_view) {
    field_of_view = new_field_of_view;

    invalidate_proj_matrix_cache();
}

void CameraNode::set_aspect_ratio(float new_aspect_ratio) {
    manual_aspect_ratio = new_aspect_ratio;

    invalidate_proj_matrix_cache();
}

void CameraNode::reset_aspect_ratio() {
    manual_aspect_ratio = std::nullopt;

    invalidate_proj_matrix_cache();
}

[[nodiscard]] glm::vec3 CameraNode::get_direction() const {
    return get_global_rotation() * FORWARD;
}

void CameraNode::set_far_distance(float far_distance) {
    this->far_distance = far_distance;

    invalidate_proj_matrix_cache();
}

[[nodiscard]] float CameraNode::get_far_distance() const {
    return far_distance;
}

void CameraNode::set_near_distance(float near_distance) {
    this->near_distance = near_distance;
    
    invalidate_proj_matrix_cache();
}

[[nodiscard]] float CameraNode::get_near_distance() const {
    return near_distance;
}

// The algorithm taken from https://web.archive.org/web/20210226045225/http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
[[nodiscard]] Frustum CameraNode::get_frustum() const {
    const glm::mat4& proj = get_proj_matrix();
    const glm::vec4 row_1 = { proj[0][0], proj[1][0], proj[2][0], proj[3][0] };
    const glm::vec4 row_2 = { proj[0][1], proj[1][1], proj[2][1], proj[3][1] };
    const glm::vec4 row_3 = { proj[0][2], proj[1][2], proj[2][2], proj[3][2] };
    const glm::vec4 row_4 = { proj[0][3], proj[1][3], proj[2][3], proj[3][3] };

    Frustum frustum;
    frustum.planes.left = Plane::from_general_form(row_4 + row_1).flipped();
    frustum.planes.right = Plane::from_general_form(row_4 - row_1).flipped();
    frustum.planes.top = Plane::from_general_form(row_4 + row_2).flipped();
    frustum.planes.bottom = Plane::from_general_form(row_4 - row_2).flipped();
    frustum.planes.near = Plane::from_general_form(row_4 + row_3).flipped();
    frustum.planes.far = Plane::from_general_form(row_4 - row_3).flipped();

    glm::mat4 model_matrix = get_global_matrix();
    frustum.planes.left = frustum.planes.left.transformed(model_matrix);
    frustum.planes.right = frustum.planes.right.transformed(model_matrix);
    frustum.planes.top = frustum.planes.top.transformed(model_matrix);
    frustum.planes.bottom = frustum.planes.bottom.transformed(model_matrix);
    frustum.planes.near = frustum.planes.near.transformed(model_matrix);
    frustum.planes.far = frustum.planes.far.transformed(model_matrix);

    return frustum;
}

void CameraNode::_on_attachment_to_tree_without_start() {
    SpatialNode::_on_attachment_to_tree_without_start();
    rs().register_camera_node(this);
}

void CameraNode::recompute_view_matrix() const noexcept {
    const glm::vec3 forward = get_global_rotation() * FORWARD;
    const glm::vec3 up = get_global_rotation() * UP;
    const glm::vec3 position = get_global_position();
    cached_view_matrix = glm::lookAt(position, position + forward, up);

    is_cached_view_matrix_valid = true;
}

void CameraNode::recompute_proj_matrix() const noexcept {
    cached_proj_matrix = glm::perspective(
        field_of_view,
        get_aspect_ratio(),
        get_near_distance(),
        get_far_distance()
    );

    is_cached_proj_matrix_valid = true;
}

void CameraNode::invalidate_view_matrix_cache() const noexcept {
    is_cached_view_matrix_valid = false;
    is_cached_view_proj_matrix_valid = false;
}

void CameraNode::invalidate_proj_matrix_cache() const noexcept {
    is_cached_proj_matrix_valid = false;
    is_cached_view_proj_matrix_valid = false;
}

[[nodiscard]] float CameraNode::get_aspect_ratio() const {
    if (manual_aspect_ratio.has_value()) {
        return *manual_aspect_ratio;
    }

    const glm::ivec2 framebuffer_size = rs().get_window().get_framebuffer_size();
    return static_cast<float>(framebuffer_size.x) / framebuffer_size.y;
}