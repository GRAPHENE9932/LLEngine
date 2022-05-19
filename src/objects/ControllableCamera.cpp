#include "ControllableCamera.hpp"

ControllableCamera::ControllableCamera(glm::vec3 position, float fov, int screen_size_x,
                                       int screen_size_y, GLFWwindow* window) :

                                       center_x(screen_size_x / 2.0), center_y(screen_size_y / 2.0),
                                       prev_mouse_x(center_x), prev_mouse_y(center_y),
                                       Camera(position, fov, static_cast<float>(screen_size_x) / screen_size_y),
                                       window(window) {

}

void ControllableCamera::update(float delta) noexcept {
    // Get mouse position.
    double cur_x, cur_y;
    glfwGetCursorPos(window, &cur_x, &cur_y);
    // Reset mouse position.
    glfwSetCursorPos(window, center_x, center_y);
    // Set rotation.
    y_rotation += static_cast<float>(mouse_sensivity * delta * (center_x - cur_x));
    x_rotation += static_cast<float>(mouse_sensivity * delta * (center_y - cur_y));

    recompute_direction();
    if (glfwGetKey(window, FORWARD_KEY) == GLFW_PRESS) {
        translation += direction * delta * forward_speed;
    }
    if (glfwGetKey(window, BACKWARD_KEY) == GLFW_PRESS) {
        translation -= direction * delta * backward_speed;
    }
    if (glfwGetKey(window, RIGHT_KEY) == GLFW_PRESS) {
        auto right = compute_right();
        translation += right * delta * strafe_speed;
    }
    if (glfwGetKey(window, LEFT_KEY) == GLFW_PRESS) {
        auto right = compute_right();
        translation -= right * delta * strafe_speed;
    }
}

glm::vec3 ControllableCamera::compute_right() const noexcept {
    return glm::vec3(
        std::sin(y_rotation - M_PI_2),
        0.0f,
        std::cos(y_rotation - M_PI_2)
    );
}
