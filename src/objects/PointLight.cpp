#include <string>

#include <GL/glew.h>

#include "PointLight.hpp"

inline std::string constr_name(const std::string& var_name, const GLuint index, const std::string& subvar_name) {
    return var_name + '[' + std::to_string(index) + "]." + subvar_name;
}

PointLight::Uniforms PointLight::get_uniforms_id(GLuint program_id, std::string var_name, GLuint index) {
    PointLight::Uniforms result;

    // Position.
    std::string position_string = var_name + '[' + std::to_string(index) + "].position";
    result.position_id = glGetUniformLocation(program_id, position_string.c_str());

    // Color.
    std::string color_string = var_name + '[' + std::to_string(index) + "].color";
    result.color_id = glGetUniformLocation(program_id, color_string.c_str());

    // Constant coefficient.
    std::string const_coeff_string = var_name + '[' + std::to_string(index) + "].const_coeff";
    result.const_coeff_id = glGetUniformLocation(program_id, const_coeff_string.c_str());

    // Linear coefficient.
    std::string linear_coeff_string = var_name + '[' + std::to_string(index) + "].linear_coeff";
    result.linear_coeff_id = glGetUniformLocation(program_id, linear_coeff_string.c_str());

    // Quadratic coefficient.
    std::string quadratic_coeff_string = var_name + '[' + std::to_string(index) + "].quadratic_coeff";
    result.quadratic_coeff_id = glGetUniformLocation(program_id, quadratic_coeff_string.c_str());

    return result;
}

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float const_coeff,
                       float linear_coeff, float quadratic_coeff) :
    position(position), color(color), const_coeff(const_coeff),
    linear_coeff(linear_coeff), quadratic_coeff(quadratic_coeff) {

}

void PointLight::set_uniforms(const PointLight::Uniforms& uniforms) const {
    glUniform3fv(uniforms.position_id, 1, &position[0]);
    glUniform3fv(uniforms.color_id, 1, &color[0]);
    glUniform1fv(uniforms.const_coeff_id, 1, &const_coeff);
    glUniform1fv(uniforms.linear_coeff_id, 1, &linear_coeff);
    glUniform1fv(uniforms.quadratic_coeff_id, 1, &quadratic_coeff);
}
