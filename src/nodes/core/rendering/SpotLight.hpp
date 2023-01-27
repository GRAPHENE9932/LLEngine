#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct SpotLight {
    struct Uniforms {
        GLuint position_id, direction_id, color_and_strength_id,
                inner_cutoff_id, outer_cutoff_id;
    };

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float inner_cutoff_angle, outer_cutoff_angle;
    float strength;

    static Uniforms get_uniforms_id(GLuint program_id, const std::string& arr_name, GLuint index);

    SpotLight() = default;
    SpotLight(const glm::vec3& position, const glm::vec3& direction,
            const glm::vec3& color, const float inner_cutoff_angle,
            const float outer_cutoff_angle, const float strength) :
            position(position), direction(direction), color(color),
            inner_cutoff_angle(inner_cutoff_angle),
            outer_cutoff_angle(outer_cutoff_angle), strength(strength) {}

    void set_uniforms(const Uniforms& uniforms) const;
};
