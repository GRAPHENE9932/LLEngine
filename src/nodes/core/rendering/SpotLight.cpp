#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "SpotLight.hpp"

SpotLight::Uniforms SpotLight::get_uniforms_id(GLuint program_id, const std::string& arr_name, GLuint index) {
    Uniforms result;

    // Position.
    std::string position_string {arr_name + '[' + std::to_string(index) + "].position"};
    result.position_id = glGetUniformLocation(program_id, position_string.c_str());

    // Direction.
    std::string direction_string {arr_name + '[' + std::to_string(index) + "].direction"};
    result.direction_id = glGetUniformLocation(program_id, direction_string.c_str());

    // Color and strength.
    std::string color_and_strength_string {arr_name + '[' + std::to_string(index) + "].color_and_strength"};
    result.color_and_strength_id = glGetUniformLocation(program_id, color_and_strength_string.c_str());

    // Inner cutoff angle.
    std::string inner_cutoff_string {arr_name + '[' + std::to_string(index) + "].inner_cutoff_angle_cos"};
    result.inner_cutoff_id = glGetUniformLocation(program_id, inner_cutoff_string.c_str());

    // Outer cutoff angle.
    std::string outer_cutoff_string {arr_name + '[' + std::to_string(index) + "].outer_cutoff_angle_cos"};
    result.outer_cutoff_id = glGetUniformLocation(program_id, outer_cutoff_string.c_str());

    return result;
}

void SpotLight::set_uniforms(const SpotLight::Uniforms& uniforms) const {
    glUniform3fv(uniforms.position_id, 1, glm::value_ptr(position));
    glUniform3fv(uniforms.direction_id, 1, glm::value_ptr(direction));

    glm::vec3 color_and_strength {color * strength};
    glUniform3fv(uniforms.color_and_strength_id, 1, &color_and_strength[0]);

    float inner_cos {std::cos(inner_cutoff_angle)};
    glUniform1fv(uniforms.inner_cutoff_id, 1, &inner_cos);

    float outer_cos {std::cos(outer_cutoff_angle)};
    glUniform1fv(uniforms.outer_cutoff_id, 1, &outer_cos);
}
