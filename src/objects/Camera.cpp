#include <glm/gtx/transform.hpp>

#include "Camera.hpp"

const glm::vec3 UP(0.0f, 1.0f, 0.0f);

Camera::Camera(glm::vec3 position, float fov, float display_ratio) : translation(position) {
    proj_matrix = glm::perspective(fov, display_ratio, 0.1f, 100.0f);
    recompute_direction();
}

glm::mat4 Camera::compute_mvp_matrix() {
    glm::mat4 view_matrix = glm::lookAt(translation,
                                        translation + direction,
                                        UP);

    return proj_matrix * view_matrix;
}

void Camera::recompute_direction() noexcept {
    direction = glm::vec3(
        std::cos(x_rotation) * std::sin(y_rotation),
        std::sin(x_rotation),
        std::cos(x_rotation) * std::cos(y_rotation)
    );
}
