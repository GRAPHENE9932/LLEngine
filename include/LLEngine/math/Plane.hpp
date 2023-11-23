#pragma once

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/transform.hpp>

namespace llengine {
struct Plane {
    glm::vec3 normal {0.0f, 1.0f, 0.0f};
    float distance = 0.0f;

    /**
     * @brief Creates the Plane object from coefficients of the
     * General Form of the Equation of a Plane (Ax+By+Cz+D=0).
     */
    [[nodiscard]] static Plane from_general_form(float a, float b, float c, float d) {
        Plane result {{a, b, c}, d};
        float magnitude = glm::length(result.normal);
        result.normal /= magnitude;
        result.distance /= magnitude;
        return result;
    }

    /**
     * @see from_general_form
     */
    [[nodiscard]] static Plane from_general_form(const glm::vec4& abcd) {
        return from_general_form(abcd.x, abcd.y, abcd.z, abcd.w);
    }

    [[nodiscard]] bool is_facing_the_point(const glm::vec3& point) const {
        float equation_result = glm::dot(normal, point) - distance;
        return equation_result >= 0.0f;
    }

    [[nodiscard]] Plane flipped() const {
        return { -normal, -distance };
    }

    [[nodiscard]] Plane transformed(const glm::mat4& matrix) const {
        glm::vec4 new_point { normal * distance, 1.0f };
        glm::vec3 new_normal { normal };
        new_point = matrix * new_point;
        new_normal = glm::transpose(glm::inverse(glm::mat3(matrix))) * new_normal;
        float new_distance = glm::dot(glm::vec3(new_point), glm::vec3(new_normal));

        return { glm::vec3(new_normal), new_distance };
    }
};
}