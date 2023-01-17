#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform {
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    [[nodiscard]] glm::mat4 calculate_matrix() const {
        return glm::translate(translation) *
            glm::scale(scale) *
            glm::toMat4(rotation);
    }
};
