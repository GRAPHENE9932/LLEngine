#pragma once

#include <string> // std::string

#include <glm/vec3.hpp> // glm::vec3
#include <GL/glew.h> // GLint

#include "nodes/core/CompleteSpatialNode.hpp"

class RenderingServer;

struct PointLightNode : public CompleteSpatialNode {
    struct Uniforms {
        GLint position_id, color_id;
        
        auto operator<=>(const Uniforms& other) const noexcept = default;
    };

    RenderingServer& rs;

    glm::vec3 color;

    PointLightNode(RenderingServer& rs, const Transform& transform = Transform());
    ~PointLightNode();

    static Uniforms get_uniforms_id(GLuint program_id, const std::string& var_name, GLuint index);
    void set_uniforms(const Uniforms& uniforms) const;
};
