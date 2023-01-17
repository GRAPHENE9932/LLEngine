#pragma once

#include <string> // std::string

#include <glm/vec3.hpp> // glm::vec3
#include <GL/glew.h> // GLint

#include "nodes/core/CompleteSpatialNode.hpp"

class RenderingServer;

struct PointLightNode : public CompleteSpatialNode {
    struct Uniforms {
        GLint position_id, color_id, diffuse_strength_id, const_coeff_id,
              linear_coeff_id, quadratic_coeff_id;
        
        auto operator<=>(const Uniforms& other) const noexcept = default;
    };

    glm::vec3 color;
    float diffuse_strength = 0.0f;
    float const_coeff = 0.0f;
    float linear_coeff = 0.0f;
    float quadratic_coeff = 0.0f;

    RenderingServer& rendering_server;

    PointLightNode(const Transform& p, RenderingServer& rs);
    ~PointLightNode();

    static Uniforms get_uniforms_id(GLuint program_id, const std::string& var_name, GLuint index);
    void set_uniforms(const Uniforms& uniforms) const;
};
