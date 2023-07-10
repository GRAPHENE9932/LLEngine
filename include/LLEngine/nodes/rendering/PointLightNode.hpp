#pragma once

#include <string> // std::string

#include <glm/vec3.hpp> // glm::vec3
#include <GL/glew.h> // GLint

#include "nodes/CompleteSpatialNode.hpp"

namespace llengine {
class RenderingServer;

struct PointLightNode : public CompleteSpatialNode {
    struct Uniforms {
        GLint position_id, color_id;
        
        auto operator<=>(const Uniforms& other) const noexcept = default;
    };

    glm::vec3 color;

    PointLightNode() = default;
    PointLightNode(const PointLightNode& other) = delete;
    PointLightNode(PointLightNode&& other) = delete;
    PointLightNode& operator=(const PointLightNode& other) = delete;
    PointLightNode& operator=(PointLightNode&& other) = delete;
    ~PointLightNode();

    void on_attachment_to_tree() final override;

    static Uniforms get_uniforms_id(GLuint program_id, const std::string& var_name, GLuint index);
    void set_uniforms(const Uniforms& uniforms) const;
};
}