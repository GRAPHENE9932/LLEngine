#pragma once

#include <memory> // std::shared_ptr

#include "common/core/Texture.hpp" // Texture
#include "DrawableNode.hpp" // DrawableNode
#include "common/core/IMesh.hpp" // IMesh
#include "common/core/Material.hpp" // Material
#include "nodes/core/SpatialNode.hpp" // SpatialNode
#include "common/core/shaders/CommonShader.hpp" // TexturedShader

class CommonDrawableNode : public DrawableNode {
public:
    std::shared_ptr<IMesh> mesh;
    std::shared_ptr<Material> material;

    CommonDrawableNode(const SpatialParams& params,
                       SceneTree& scene_tree,
                       const std::shared_ptr<Material>& material,
                       const std::shared_ptr<IMesh>& mesh);

    void draw() override;
    void register_myself(SpatialNode* parent) override;
    GLuint get_program_id() const override;
};
