#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
    glm::vec3 translation;
    float x_rotation = 0.0f, y_rotation = 0.0f;

    Camera(glm::vec3 position, float fov, float display_ratio);

    glm::mat4 compute_mvp_matrix();

protected:
    glm::mat4 proj_matrix;
    glm::vec3 direction;

    void recompute_direction() noexcept;
};
