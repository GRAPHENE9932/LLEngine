#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

class SpatialObject {
public:
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;

    SpatialObject() = default;
    SpatialObject(const glm::vec3& translation, const glm::vec3& scale,
            const glm::quat& quat = glm::quat()) : translation(translation),
            rotation(quat), scale(scale) {};

    glm::mat4 compute_matrix() const noexcept;
};
