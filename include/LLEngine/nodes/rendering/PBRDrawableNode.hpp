#pragma once

#include <memory> // std::shared_ptr

#include "DrawableNode.hpp" // DrawableNode
#include "rendering/Mesh.hpp"
#include "rendering/Material.hpp" // Material

class PBRDrawableNode : public DrawableNode {
public:
    std::shared_ptr<const Mesh> mesh;
    std::shared_ptr<Material> material;

    PBRDrawableNode(
        RenderingServer& rs,
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<const Mesh>& mesh,
        const Transform& transform = Transform()
    );

    void draw() override;
    GLuint get_program_id() const override;
};
