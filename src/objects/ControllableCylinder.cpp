#include "ControllableCylinder.hpp"

ControllableCylinder::ControllableCylinder(Camera* camera, GLFWwindow* window,
                                           glm::vec3 position, float height, float radius) :

                                           cylinder(position, height, radius),
                                           camera(camera), window(window) {
    // Set center_x and center_y.
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    center_x = window_width / 2.0;
    center_y = window_height / 2.0;
}

void ControllableCylinder::update(float delta) {
    glm::vec3 moving_direction = glm::vec3(camera->direction.x, 0.0f, camera->direction.z);

    camera->recompute_direction();
    if (glfwGetKey(window, FORWARD_KEY) == GLFW_PRESS) {
        cylinder.position += moving_direction * delta * forward_speed;
    }
    if (glfwGetKey(window, BACKWARD_KEY) == GLFW_PRESS) {
        cylinder.position -= moving_direction * delta * backward_speed;
    }
    if (glfwGetKey(window, RIGHT_KEY) == GLFW_PRESS) {
        auto right = compute_right();
        cylinder.position += right * delta * strafe_speed;
    }
    if (glfwGetKey(window, LEFT_KEY) == GLFW_PRESS) {
        auto right = compute_right();
        cylinder.position -= right * delta * strafe_speed;
    }
    if (on_floor && glfwGetKey(window, JUMP_KEY) == GLFW_PRESS) {
        jump();
    }

    cylinder.position.y += vertical_velocity;
}

void ControllableCylinder::update_camera(float delta) {
    camera->translation = cylinder.highest_point();
    update_camera_orientation(delta);
}

void ControllableCylinder::add_vertical_velocity(float difference) {
    vertical_velocity += difference;
}

void ControllableCylinder::zero_out_vertical_velocity() {
    vertical_velocity = 0.0f;
}

void ControllableCylinder::landed() {
    on_floor = true;
}

void ControllableCylinder::update_camera_orientation(float delta) {
    // Get mouse position.
    double cur_x, cur_y;
    glfwGetCursorPos(window, &cur_x, &cur_y);
    // Reset mouse position.
    glfwSetCursorPos(window, center_x, center_y);
    // Set rotation.
    camera->y_rotation += static_cast<float>(mouse_sensivity * delta * (center_x - cur_x));
    if (camera->y_rotation < 0.0f)
        camera->y_rotation += glm::radians(360.0f);
    else if (camera->y_rotation > glm::radians(360.0f))
        camera->y_rotation -= glm::radians(360.0f);

    camera->x_rotation += static_cast<float>(mouse_sensivity * delta * (center_y - cur_y));
    if (camera->x_rotation > MAX_X_ROTATION)
        camera->x_rotation = MAX_X_ROTATION;
    else if (camera->x_rotation < MIN_X_ROTATION)
        camera->x_rotation = MIN_X_ROTATION;
}

void ControllableCylinder::jump() {
    vertical_velocity += jump_force;
    on_floor = false;
}

glm::vec3 ControllableCylinder::compute_right() const noexcept {
    return glm::vec3(
        std::sin(camera->y_rotation - M_PI_2),
        0.0f,
        std::cos(camera->y_rotation - M_PI_2)
    );
}
