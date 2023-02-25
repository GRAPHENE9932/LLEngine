#pragma once

#include <memory> // std::shared_ptr

#include "DrawableNode.hpp" // DrawableNode
#include "common/core/Mesh.hpp"
#include "common/core/Material.hpp" // Material

class PBRDrawableNode : public DrawableNode {
public:
    std::shared_ptr<const Mesh> mesh;
    std::shared_ptr<Material> material;

    PBRDrawableNode(const Transform& p,
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<const Mesh>& mesh);

    void draw() override;
    GLuint get_program_id() const override;
};
