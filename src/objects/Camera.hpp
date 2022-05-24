#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
    glm::vec3 translation;
    glm::vec3 direction;
    float x_rotation = 0.0f, y_rotation = 0.0f;

    Camera(glm::vec3 position, float fov, float display_ratio);

    glm::mat4 compute_mvp_matrix(glm::mat4& view_matrix);
    void recompute_direction() noexcept;

protected:
    glm::mat4 proj_matrix;
};
