#pragma once

#include <memory> // std::shared_ptr

#include "DrawableCompleteSpatialNode.hpp" // DrawableNode
#include "rendering/Mesh.hpp"
#include "rendering/Material.hpp" // Material
#include "datatypes.hpp"

namespace llengine {
class PBRDrawableNode : public DrawableCompleteSpatialNode {
public:
    PBRDrawableNode();
    PBRDrawableNode(
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<const Mesh>& mesh
    );

    void draw() override;
    void draw_to_shadow_map() override;
    ShaderID get_program_id() const override;

    void set_mesh(const std::shared_ptr<const Mesh>& mesh);
    void set_material(const std::shared_ptr<Material>& material);

    [[nodiscard]] const Mesh& get_mesh() const;
    [[nodiscard]] const Material& get_material() const;

    [[nodiscard]] virtual bool is_outside_the_frustum(const Frustum& frustum) const final override;

    void copy_to(Node& node) const override;
    std::unique_ptr<Node> copy() const override;

private:
    std::shared_ptr<const Mesh> mesh = nullptr;
    std::shared_ptr<Material> material = nullptr;
};
}