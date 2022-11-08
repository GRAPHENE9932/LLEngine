#pragma once

#include <memory> // std::shared_ptr

#include "DrawableNode.hpp" // DrawableNode
#include "common/core/IMesh.hpp" // IMesh
#include "common/core/Material.hpp" // Material

class RenderingServer;

class CommonDrawableNode : public DrawableNode {
public:
    std::shared_ptr<IMesh> mesh;
    std::shared_ptr<Material> material;

    CommonDrawableNode(const SpatialParams& p, RenderingServer& rs,
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<IMesh>& mesh);

    void draw() override;
    GLuint get_program_id() const override;
};
