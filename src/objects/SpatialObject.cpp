#include <glm/gtx/transform.hpp>

#include "SpatialObject.hpp"

glm::mat4 SpatialObject::compute_matrix() const noexcept {
    return glm::translate(translation) * glm::scale(scale) * glm::toMat4(rotation);
}
