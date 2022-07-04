#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct PointLight {
    struct Uniforms {
        GLuint position_id, color_id, diffuse_strength_id, const_coeff_id,
                linear_coeff_id, quadratic_coeff_id;
    };

    glm::vec3 position;
    glm::vec3 color;
    float diffuse_strength = 0.0f;
    float const_coeff = 0.0f, linear_coeff = 0.0f, quadratic_coeff = 0.0f;

    /// Just like position, but relative to the view (multiplied with view matrix).
    glm::vec3 overlay_position;

    static Uniforms get_uniforms_id(GLuint program_id, std::string var_name, GLuint index);

    PointLight() = default;
    PointLight(const glm::vec3& position, const glm::vec3& color, float diffuse_strength,
            float const_coeff, float linear_coeff, float quadratic_coeff) :
            position(position), color(color), diffuse_strength(diffuse_strength),
            const_coeff(const_coeff), linear_coeff(linear_coeff), quadratic_coeff(quadratic_coeff) {}

    void set_uniforms(const Uniforms& uniforms, const bool overlay = false) const;
    inline void calc_overlay_position(const glm::mat4& view_matrix) {
        overlay_position = view_matrix * glm::vec4(position, 1.0f);
    }
};
