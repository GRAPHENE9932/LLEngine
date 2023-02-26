#include "PointLightNode.hpp" // PointLightNode
#include "RenderingServer.hpp" // RenderingServer

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <string> // std::string

PointLightNode::PointLightNode(RenderingServer& rs, const Transform& transform) :
    CompleteSpatialNode(transform), rs(rs) {
    rs.register_point_light(this);
}

PointLightNode::~PointLightNode() {
    rs.unregister_point_light(this);
}

PointLightNode::Uniforms PointLightNode::get_uniforms_id(
    GLuint program_id, const std::string& var_name, GLuint index) {
    PointLightNode::Uniforms result;

    // Position.
    std::string position_string {var_name + '[' + std::to_string(index) + "].position"};
    result.position_id = glGetUniformLocation(program_id, position_string.c_str());

    // Color.
    std::string color_string {var_name + '[' + std::to_string(index) + "].color"};
    result.color_id = glGetUniformLocation(program_id, color_string.c_str());

    // Diffuse strength.
    std::string diffuse_strength_string {var_name + '[' + std::to_string(index) + "].diffuse_strength"};
    result.diffuse_strength_id = glGetUniformLocation(program_id, diffuse_strength_string.c_str());

    // Constant coefficient.
    std::string const_coeff_string {var_name + '[' + std::to_string(index) + "].const_coeff"};
    result.const_coeff_id = glGetUniformLocation(program_id, const_coeff_string.c_str());

    // Linear coefficient.
    std::string linear_coeff_string {var_name + '[' + std::to_string(index) + "].linear_coeff"};
    result.linear_coeff_id = glGetUniformLocation(program_id, linear_coeff_string.c_str());

    // Quadratic coefficient.
    std::string quadratic_coeff_string {var_name + '[' + std::to_string(index) + "].quadratic_coeff"};
    result.quadratic_coeff_id = glGetUniformLocation(program_id, quadratic_coeff_string.c_str());

    return result;
}

void PointLightNode::set_uniforms(const PointLightNode::Uniforms& uniforms) const {
    glUniform3fv(uniforms.position_id, 1, glm::value_ptr(get_translation()));
    glUniform3fv(uniforms.color_id, 1, glm::value_ptr(color));
    glUniform1fv(uniforms.diffuse_strength_id, 1, &diffuse_strength);
    glUniform1fv(uniforms.const_coeff_id, 1, &const_coeff);
    glUniform1fv(uniforms.linear_coeff_id, 1, &linear_coeff);
    glUniform1fv(uniforms.quadratic_coeff_id, 1, &quadratic_coeff);
}
