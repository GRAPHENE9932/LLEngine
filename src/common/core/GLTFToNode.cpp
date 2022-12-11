#include "GLTF.hpp" // GLTF
#include "common/core/KTXTexture.hpp" // KTXTexture
#include "nodes/core/rendering/CommonDrawableNode.hpp" // CommonDrawableNode

std::unique_ptr<SpatialNode> to_node(RenderingServer& rs, const GLTF::Node& gltf_node,
    const GLTF& gltf, const std::vector<std::shared_ptr<IMesh>>& meshes,
    const std::vector<std::shared_ptr<Material>>& materials) {
    std::unique_ptr<SpatialNode> result;

    if (gltf_node.mesh_index.has_value()) {
        result = std::make_unique<CommonDrawableNode>(
            gltf_node.spatial_params,
            rs,
            materials.at(gltf.meshes.at(*gltf_node.mesh_index).material_index),
            meshes.at(*gltf_node.mesh_index)
        );
    }
    else {
        result = std::make_unique<::SpatialNode>(gltf_node.spatial_params);
    }

    result->name = gltf_node.name;

    for (const GLTF::Node& cur_child : gltf_node.children) {
        result->add_child(std::move(
            *to_node(
                rs,
                cur_child,
                gltf,
                meshes,
                materials
            ).release()
        ));
    }

    return result;
}

template<typename T>
std::shared_ptr<Mesh<T>> construct_mesh(const GLTF::MeshParameters& mesh_params) {
    std::shared_ptr<Mesh<T>> result = std::make_shared<Mesh<T>>();

    if (std::holds_alternative<std::monostate>(mesh_params.indices))
        result->set_indices({});
    result->set_indices(std::move(std::get<std::vector<T>>(mesh_params.indices)));
    result->set_vertices(std::move(mesh_params.vertices));
    result->set_uvs(std::move(mesh_params.uvs));
    result->set_normals(std::move(mesh_params.normals));
    result->set_tangents(std::move(mesh_params.tangents));

    return result;
}

std::optional<Material::TextureInfo>
construct_texture_info(const std::optional<BasicMaterial<uint32_t>::TextureInfo>& tex_info_params,
        const std::vector<std::shared_ptr<Texture>>& textures) {
    if (!tex_info_params.has_value())
        return std::nullopt;

    return Material::TextureInfo({
        textures.at(tex_info_params->texture),
        tex_info_params->uv_offset,
        tex_info_params->uv_scale
    });
}

std::shared_ptr<Material> construct_material(const BasicMaterial<uint32_t>& mat_params,
        const std::vector<std::shared_ptr<Texture>>& textures) {
    std::shared_ptr<Material> result = std::make_shared<Material>();

    result->base_color = {
        construct_texture_info(mat_params.base_color.texture, textures),
        mat_params.base_color.factor
    };
    result->metallic_roughness = {
        construct_texture_info(mat_params.metallic_roughness.texture, textures),
        mat_params.metallic_roughness.metallic_factor,
        mat_params.metallic_roughness.roughness_factor
    };
    if (mat_params.normal_map.has_value()) {
        result->normal_map = {
            *construct_texture_info(mat_params.normal_map->texture, textures),
            mat_params.normal_map->scale
        };
    }
    else {
        result->normal_map = std::nullopt;
    }
    result->occlusion = {
        construct_texture_info(mat_params.occlusion.texture, textures),
        mat_params.occlusion.strength
    };
    result->emmisive = {
        construct_texture_info(mat_params.emmisive.texture, textures),
        mat_params.emmisive.factor
    };

    return result;
}

std::unique_ptr<::SpatialNode> GLTF::to_node(RenderingServer& rs) const {
    // Construct meshes.
    std::vector<std::shared_ptr<IMesh>> meshes;
    meshes.reserve(this->meshes.size());
    for (auto& cur_mesh_params : this->meshes) {
        if (std::holds_alternative<std::vector<uint32_t>>(cur_mesh_params.indices))
            meshes.push_back(construct_mesh<uint32_t>(cur_mesh_params));
        else
            meshes.push_back(construct_mesh<uint16_t>(cur_mesh_params));
    }

    // Construct textures.
    std::vector<std::shared_ptr<Texture>> textures;
    textures.reserve(this->textures.size());
    for (auto& cur_tex_params : this->textures)
        textures.push_back(std::make_shared<KTXTexture>(cur_tex_params));

    // Construct materials.
    std::vector<std::shared_ptr<Material>> materials;
    materials.reserve(this->materials.size());
    for (const auto& cur_mat_params : this->materials)
        materials.push_back(construct_material(cur_mat_params, textures));

    // Convert glTF to node.
    if (this->nodes.size() == 1) {
        return ::to_node(
            rs,
            this->nodes[0],
            *this,
            meshes,
            materials
        );
    }
    else if (this->nodes.size() > 1) {
        // We have multiple root nodes in this glTF, but
        // we must return only one root node.
        // So, create the spatial node with default
        // spatial parameters and make it root.
        auto result = std::make_unique<SpatialNode>(SpatialNode::SpatialParams());
        for (const auto& cur_gltf_node : this->nodes) {
            result->add_child(std::move(
                *::to_node(
                    rs,
                    cur_gltf_node,
                    *this,
                    meshes,
                    materials
                ).release()
            ));
        }
        return result;
    }
    else {
        return std::make_unique<SpatialNode>(SpatialNode::SpatialParams());
    }
}
