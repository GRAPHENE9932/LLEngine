#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace llengine {
struct Transform {
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    [[nodiscard]] glm::mat4 calculate_matrix() const {
        return glm::scale(
            glm::translate(
                glm::mat4(1.0f),
                translation
            ) * glm::mat4_cast(rotation),
            scale
        );
    }

    /**
     * @brief Combines multiple transforms similarly to
     * matrix multiplication. Keep in mind that transformation
     * order is opposite to operand order, just like in
     * matrix multiplication.
     */
    Transform operator*=(const Transform& other) noexcept {
        translation += other.translation;
        scale *= other.scale;
        rotation *= other.rotation;
        return *this;
    }

    Transform operator*(const Transform& other) const noexcept {
        Transform result(*this);
        result *= other;
        return result;
    }

    Transform operator/=(const Transform& other) noexcept {
        translation -= other.translation;
        scale /= other.scale;
        rotation *= glm::inverse(other.rotation);
        return *this;
    }

    Transform operator/(const Transform& other) const noexcept {
        Transform result(*this);
        result /= other;
        return result;
    }

    bool operator==(const Transform& other) const noexcept {
        return translation == other.translation &&
            scale == other.scale &&
            rotation == other.rotation;
    }

    bool operator!=(const Transform& other) const noexcept {
        return !(*this == other);
    }
};
}