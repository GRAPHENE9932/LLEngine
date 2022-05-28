#pragma once

#include <glm/vec3.hpp>

struct PointLight {
    struct Uniforms {
        GLuint position_id, color_id, const_coeff_id, linear_coeff_id,
            quadratic_coeff_id;
    };

    glm::vec3 position;
    glm::vec3 color;
    float const_coeff = 0.0f, linear_coeff = 0.0f, quadratic_coeff = 0.0f;

    static Uniforms get_uniforms_id(GLuint program_id, std::string var_name, GLuint index);

    PointLight() = default;
    PointLight(glm::vec3 position, glm::vec3 color, float const_coeff,
               float linear_coeff, float quadratic_coeff);
    void set_uniforms(const Uniforms& uniforms) const;
};