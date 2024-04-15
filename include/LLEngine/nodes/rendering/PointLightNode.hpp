#pragma once

#include <string> // std::string

#include <glm/vec3.hpp> // glm::vec3

#include "nodes/CompleteSpatialNode.hpp"
#include "datatypes.hpp"

namespace llengine {
class RenderingServer;

struct PointLightNode : public CompleteSpatialNode {
    struct Uniforms {
        ShaderUniformID position_id, color_id;
        
        auto operator<=>(const Uniforms& other) const noexcept = default;
    };

    glm::vec3 color;

    PointLightNode() = default;
    PointLightNode(const PointLightNode& other) = delete;
    PointLightNode(PointLightNode&& other) = delete;
    PointLightNode& operator=(const PointLightNode& other) = delete;
    PointLightNode& operator=(PointLightNode&& other) = delete;
    ~PointLightNode();

    void _on_attachment_to_tree_without_start() final override;

    static Uniforms get_uniforms_id(ShaderID program_id, const std::string& var_name, std::uint32_t index);
    void set_uniforms(const Uniforms& uniforms) const;
};
}