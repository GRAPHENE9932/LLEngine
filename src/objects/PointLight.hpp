#pragma once

#include <glm/vec3.hpp>

// Also, change src/shaders/textured_fragment.glsl:TX_DRW_POINT_LIGHTS_AMOUNT
constexpr GLuint POINT_LIGHTS_AMOUNT = 2;

struct PointLight {
public:
    struct Uniforms {
        GLuint position_id, color_id, diffuse_strength_id, const_coeff_id,
                linear_coeff_id, quadratic_coeff_id;
    };

    glm::vec3 position;
    /// Just like position, but relative to the view (multiplied with view matrix).
    glm::vec3 overlay_position;
    glm::vec3 color;
    float diffuse_strength = 0.0f;
    float const_coeff = 0.0f, linear_coeff = 0.0f, quadratic_coeff = 0.0f;

    static Uniforms get_uniforms_id(GLuint program_id, std::string var_name, GLuint index);

    PointLight() = default;
    PointLight(glm::vec3 position, glm::vec3 color, float diffuse_strength,
            float const_coeff, float linear_coeff, float quadratic_coeff);
    void set_uniforms(const Uniforms& uniforms, const bool overlay = false) const;
};
