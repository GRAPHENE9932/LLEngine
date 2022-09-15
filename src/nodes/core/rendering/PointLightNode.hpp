#pragma once

#include <string> // std::string

#include <glm/vec3.hpp> // glm::vec3
#include <GL/glew.h> // GLint

#include "nodes/core/SpatialNode.hpp" // SpatialNode

struct PointLightNode : public SpatialNode {
    struct Uniforms {
        GLint position_id, color_id, diffuse_strength_id, const_coeff_id,
              linear_coeff_id, quadratic_coeff_id;
    };

    glm::vec3 color;
    float diffuse_strength = 0.0f;
    float const_coeff = 0.0f, linear_coeff = 0.0f, quadratic_coeff = 0.0f;

    PointLightNode(const SpatialParams& p, SceneTree& tree) :
            SpatialNode(p, tree) {}

    void register_myself(SpatialNode* parent) override;

    static Uniforms get_uniforms_id(GLuint program_id, std::string var_name, GLuint index);
    void set_uniforms(const Uniforms& uniforms) const;
};
