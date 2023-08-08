#pragma once

#include <memory> // std::shared_ptr

#include "DrawableCompleteSpatialNode.hpp" // DrawableNode
#include "nodes/CompleteSpatialNode.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Material.hpp" // Material

namespace llengine {
class PBRDrawableNode : public DrawableCompleteSpatialNode {
public:
    std::shared_ptr<const Mesh> mesh = nullptr;
    std::shared_ptr<Material> material = nullptr;

    PBRDrawableNode();
    PBRDrawableNode(
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<const Mesh>& mesh
    );

    void draw() override;
    void draw_to_shadow_map() override;
    GLuint get_program_id() const override;

    void copy_to(Node& node) const override;
    std::unique_ptr<Node> copy() const override;
};
}