#include "RenderingServer.hpp" // RenderingServer
#include "SpectatorCameraNode.hpp" // SpectatorCameraNode

#include <glm/ext/vector_int2.hpp> // glm::ivec2
#include <glm/ext/vector_double2.hpp> // glm::dvec2
#include <glm/ext/quaternion_exponential.hpp> // glm::pow
#include <GLFW/glfw3.h> // glfwGetCursorPos, glfwSetCursorPos

constexpr glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 RIGHT(-1.0f, 0.0f, 0.0f);

SpectatorCameraNode::SpectatorCameraNode(RenderingServer& rs,
    const SpatialNode::SpatialParams& p, float display_ratio, float fov) :
    CameraNode(rs, p, display_ratio, fov), rendering_server(rs) {
    rs.get_window().disable_cursor();
}

void SpectatorCameraNode::update() {
    CameraNode::update();

    if (rendering_server.get_window().is_initialized()) {
        update_rotation();
        update_position();
    }
}

/// Angle axis to quaternion conversion, but only around the
/// X axis (1.0, 0.0, 0.0).
/// More performant than angle axis with arbitrary axis.
glm::quat angle_axis_x(float angle) {
    // Angle-axis to quaternion formula:
    // qw = cos(angle / 2)
    // qx = x * sin(angle / 2)
    // qy = y * sin(angle / 2)
    // qz = z * sin(angle / 2).
    // With known axis (1.0, 0.0, 0.0) we get:
    // qw = cos(angle / 2)
    // qx = sin(angle / 2)
    // qy = 0
    // qz = 0
    return {
        std::cos(angle * 0.5f),
        std::sin(angle * 0.5f),
        0.0f,
        0.0f
    };
}

/// Angle axis to quaternion conversion, but only around the
/// Y axis (0.0, 1.0, 0.0).
/// More performant than angle axis with arbitrary axis.
glm::quat angle_axis_y(float angle) {
    // Angle-axis to quaternion formula:
    // qw = cos(angle / 2)
    // qx = x * sin(angle / 2)
    // qy = y * sin(angle / 2)
    // qz = z * sin(angle / 2).
    // With known axis (0.0, 1.0, 0.0) we get:
    // qw = cos(angle / 2)
    // qx = 0
    // qy = sin(angle / 2)
    // qz = 0
    return {
        std::cos(angle * 0.5f),
        0.0f,
        std::sin(angle * 0.5f),
        0.0f
    };
}

/// Moves arbitrary angle in range [-PI; PI] without
/// changing the angle.
void move_angle_in_bounds(float& angle) {
    if (angle > M_PIf)
        angle -= 2.0f * M_PIf;
    else if (angle < -M_PIf)
        angle += 2.0f * M_PIf;
}

void clamp_x_angle(float& x_angle) {
    move_angle_in_bounds(x_angle);
    
    if (x_angle > glm::radians(90.0f))
        x_angle = glm::radians(90.0f);
    else if (x_angle < glm::radians(-90.0f))
        x_angle = glm::radians(-90.0f);
}

void SpectatorCameraNode::update_rotation() {
    auto& window {rendering_server.get_window()};

    // Get the current cursor position.
    glm::dvec2 cursor_pos = window.get_cursor_position();
    
    // Set cursor to the center.
    const glm::dvec2 center = static_cast<glm::dvec2>(
        window.get_window_size()
    ) / 2.0;
    window.set_cursor_position(center);

    // Calculate and set new rotation.
    x_angle += sensivity * static_cast<float>(cursor_pos.y - center.y);
    y_angle -= sensivity * static_cast<float>(cursor_pos.x - center.x);
    clamp_x_angle(x_angle);
    move_angle_in_bounds(y_angle);
    set_rotation(angle_axis_y(y_angle) * angle_axis_x(x_angle));
}

void SpectatorCameraNode::update_position() {
    // Compute directions.
    const glm::vec3 forward_dir = get_rotation() * FORWARD;
    const glm::vec3 right_dir = get_rotation() * RIGHT;

    // Some aliases.
    const auto& window {rendering_server.get_window()};
    const auto& delta {rendering_server.get_delta_time()};

    if (window.is_key_pressed(FORWARD_KEY)) {
        translate(forward_dir * delta * speed);
    }
    if (window.is_key_pressed(BACKWARD_KEY)) {
        translate(forward_dir * delta * -speed);
    }
    if (window.is_key_pressed(RIGHT_KEY)) {
        translate(right_dir * delta * speed);
    }
    if (window.is_key_pressed(LEFT_KEY)) {
        translate(right_dir * delta * -speed);
    }
}
