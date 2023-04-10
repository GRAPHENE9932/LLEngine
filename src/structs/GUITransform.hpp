#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct GUITransform {
    glm::vec2 position_anchor {0.0f, 0.0f}; // (0, 0) is left-top, (1, 1) is right-bottom.
    glm::vec2 position_offset {0.0f, 0.0f}; // Offset from anchor in pixels.
    float z_coordinate {0.0f};

    /**
     * @brief Combines multiple transforms similarly to
     * matrix multiplication. Keep in mind that transformation
     * order is opposite to operand order, just like in
     * matrix multiplication.
     */
    const GUITransform& operator*=(const GUITransform& other) noexcept {
        position_anchor += (1.0f - position_anchor) * other.position_anchor;
        position_offset += other.position_offset;
        z_coordinate += other.z_coordinate;
        return *this;
    }

    GUITransform operator*(const GUITransform& other) const noexcept {
        GUITransform result(*this);
        result *= other;
        return result;
    }

    [[nodiscard]] glm::vec3 to_opengl_position(glm::u32vec2 viewport_size) const noexcept {
        glm::vec2 result {position_anchor};
        result += glm::vec2(position_offset / glm::vec2(viewport_size));
        result = result * 2.0f - 1.0f; // Go from [0; 1] range to [-1; 1].
        result.y = -result.y;
        return glm::vec3(result, z_coordinate);
    }
};