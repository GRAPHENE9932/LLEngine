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

    glm::vec3 overlay_position;
    glm::vec3 overlay_direction;

    static Uniforms get_uniforms_id(GLuint program_id, std::string arr_name, GLuint index);

    SpotLight() = default;
    SpotLight(const glm::vec3& position, const glm::vec3& direction,
            const glm::vec3& color, const float inner_cutoff_angle,
            const float outer_cutoff_angle, const float strength) :
            position(position), direction(direction), color(color),
            inner_cutoff_angle(inner_cutoff_angle),
            outer_cutoff_angle(outer_cutoff_angle), strength(strength) {}

    void set_uniforms(const Uniforms& uniforms, const bool overlay = false) const;
    inline void calc_overlay_props(const glm::mat4& view_matrix) {
        overlay_position = view_matrix * glm::vec4(position, 1.0f);
        overlay_direction = view_matrix * glm::vec4(direction, 0.0f);
    }
};
