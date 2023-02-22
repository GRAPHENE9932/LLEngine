#pragma once

#include <memory> // std::shared_ptr

#include "DrawableNode.hpp" // DrawableNode
#include "common/core/Mesh.hpp"
#include "common/core/Material.hpp" // Material

class CommonDrawableNode : public DrawableNode {
public:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    CommonDrawableNode(const Transform& p,
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<Mesh>& mesh);

    void draw() override;
    GLuint get_program_id() const override;
};
