#include "GLTF.hpp" // GLTF
#include "common/core/Mesh.hpp"
#include "common/core/KTXTexture.hpp"
#include "structs/shapes/Shape.hpp"
#include "structs/shapes/BoxShape.hpp"
#include "structs/shapes/SphereShape.hpp"
#include "structs/shapes/CylinderShape.hpp"
#include "nodes/core/rendering/CommonDrawableNode.hpp" // CommonDrawableNode
#include "nodes/core/physics/bullet/BulletRigidBodyNode.hpp"
#include "utils/json_conversion.hpp"

#include <cassert>
#include <algorithm>

// Packed parameters used across functions.
struct ConstructionEnvironment {
    const GLTF& gltf;
    const std::vector<std::shared_ptr<Mesh>>& meshes;
    const std::vector<std::shared_ptr<Material>>& materials;
    std::vector<std::shared_ptr<Shape>> shapes_pool;
};

bool GLTF::Node::is_drawable() const {
    return mesh_index.has_value();
}

bool GLTF::Node::is_rigid_body() const {
    return extras.has_value() && extras->contains("collider_type");
}

std::unique_ptr<SpatialNode> to_node(
    ConstructionEnvironment& constr_env, const GLTF::Node& gltf_node
);

template<bool with_children = true>
std::unique_ptr<CommonDrawableNode> construct_common_drawable(
    ConstructionEnvironment& constr_env, const GLTF::Node& gltf_node
) {
    assert(gltf_node.is_drawable());

    auto result = std::make_unique<CommonDrawableNode>(
        gltf_node.transform,
        constr_env.materials.at(constr_env.gltf.meshes.at(gltf_node.mesh_index.value()).material_index),
        constr_env.meshes.at(*gltf_node.mesh_index)
    );

    result->set_name(gltf_node.name);

    if constexpr (with_children) {
        for (const GLTF::Node& cur_child : gltf_node.children) {
            result->add_child(std::move(
                *to_node(constr_env, cur_child).release()
            ));
        }
    }

    return result;
}

std::unique_ptr<CompleteSpatialNode> construct_complete_spatial(
    ConstructionEnvironment& constr_env, const GLTF::Node& gltf_node
) {
    auto result = std::make_unique<CompleteSpatialNode>(gltf_node.transform);

    result->set_name(gltf_node.name);

    for (const GLTF::Node& cur_child : gltf_node.children) {
        result->add_child(std::move(
            *to_node(constr_env, cur_child).release()
        ));
    }

    return result;
}

// May return a shape from pool.
std::shared_ptr<Shape>& extract_shape(
    ConstructionEnvironment& constr_env, const GLTF::Node& gltf_node
) {
    const std::string& collider_type {
        gltf_node.extras.value().at("collider_type").get<std::string>()
    };

    std::unique_ptr<Shape> shape;

    if (collider_type == "BOX") {
        shape = std::make_unique<BoxShape>(
            gltf_node.extras.value().at("extents").get<glm::vec3>()
        );
    }
    else if (collider_type == "SPHERE") {
        shape = std::make_unique<SphereShape>(
            gltf_node.extras.value().at("radius").get<float>()
        );
    }
    else if (collider_type == "CYLINDER") {
        shape = std::make_unique<CylinderShape>(
            gltf_node.extras.value().at("radius").get<float>(),
            gltf_node.extras.value().at("radius").get<float>(),
            gltf_node.extras.value().at("height").get<float>()
        );
    }
    else {
        throw std::runtime_error("Invalid collider_type in extras.");
    }

    const auto iter = std::find_if(
        constr_env.shapes_pool.begin(), constr_env.shapes_pool.end(),
        [&shape] (const std::shared_ptr<Shape>& other) -> bool {
            return *shape == *other;
        }
    );

    if (iter == constr_env.shapes_pool.end()) {
        constr_env.shapes_pool.push_back(std::shared_ptr<Shape>(
            shape.release()
        ));
        return constr_env.shapes_pool.back();
    }
    else {
        return *iter;
    }
}

std::unique_ptr<BulletRigidBodyNode> construct_bullet_rigid_body(
    ConstructionEnvironment& constr_env, const GLTF::Node& gltf_node
) {
    assert(gltf_node.is_rigid_body());

    auto result = std::make_unique<BulletRigidBodyNode>(
        gltf_node.transform,
        extract_shape(constr_env, gltf_node),
        get_optional<float>(gltf_node.extras.value(), "mass").value_or(0.0f)
    );

    if (gltf_node.is_drawable()) {
        // If our GLTF::Node is rigid body and drawable at the same time,
        // then add the drawable component as a child.
        auto drawable {construct_common_drawable<false>(constr_env, gltf_node)};
        drawable->set_transform(Transform());
        drawable->set_name(drawable->get_name() + "_drawable");

        result->add_child(std::move(*drawable.release()));
    }

    result->set_name(gltf_node.name);

    for (const GLTF::Node& cur_child : gltf_node.children) {
        result->add_child(std::move(
            *to_node(constr_env, cur_child).release()
        ));
    }

    return result;
}

std::unique_ptr<SpatialNode> to_node(
    ConstructionEnvironment& constr_env, const GLTF::Node& gltf_node
) {
    if (gltf_node.is_rigid_body()) {
        return construct_bullet_rigid_body(constr_env, gltf_node);
    }
    else if (gltf_node.is_drawable()) {
        return construct_common_drawable(constr_env, gltf_node);
    }
    else {
        return construct_complete_spatial(constr_env, gltf_node);
    }
}

std::shared_ptr<Mesh> construct_mesh(const GLTF::MeshParameters& mesh_params) {
    std::shared_ptr<Mesh> result = std::make_shared<Mesh>();

    if (std::holds_alternative<std::vector<uint16_t>>(mesh_params.indices)) {
        result->set_indices(std::get<std::vector<uint16_t>>(mesh_params.indices));
    }
    else if (std::holds_alternative<std::vector<uint32_t>>(mesh_params.indices)) {
        result->set_indices(std::get<std::vector<uint32_t>>(mesh_params.indices));
    }

    result->set_vertices(mesh_params.vertices);
    
    if (mesh_params.uvs.has_value()) {
        result->set_uvs(*mesh_params.uvs);
    }

    if (mesh_params.normals.has_value()) {
        result->set_normals(*mesh_params.normals);
    }

    if (mesh_params.tangents.has_value()) {
        result->set_tangents(*mesh_params.tangents);
    }

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

std::optional<Material::SingleChannelTextureInfo>
construct_texture_info(const std::optional<BasicMaterial<uint32_t>::SingleChannelTextureInfo>& tex_info_params,
        const std::vector<std::shared_ptr<Texture>>& textures) {
    if (!tex_info_params.has_value())
        return std::nullopt;

    return Material::SingleChannelTextureInfo({
        textures.at(tex_info_params->texture),
        tex_info_params->uv_offset,
        tex_info_params->uv_scale,
        tex_info_params->channel
    });
}

std::shared_ptr<Material> construct_material(const BasicMaterial<uint32_t>& mat_params,
        const std::vector<std::shared_ptr<Texture>>& textures) {
    std::shared_ptr<Material> result = std::make_shared<Material>();

    result->base_color_texture = construct_texture_info(mat_params.base_color_texture, textures);
    result->base_color_factor = mat_params.base_color_factor;

    result->emissive_texture = construct_texture_info(mat_params.emissive_texture, textures);
    result->emissive_factor = mat_params.emissive_factor;

    result->ambient_occlusion_texture = construct_texture_info(mat_params.ambient_occlusion_texture, textures);
    result->ambient_occlusion_factor = mat_params.ambient_occlusion_factor;

    result->metallic_texture = construct_texture_info(mat_params.metallic_texture, textures);
    result->metallic_factor = mat_params.metallic_factor;
    
    result->roughness_texture = construct_texture_info(mat_params.roughness_texture, textures);
    result->roughness_factor = mat_params.roughness_factor;

    if (mat_params.normal_map.has_value()) {
        result->normal_map = {
            *construct_texture_info(mat_params.normal_map->texture, textures),
            mat_params.normal_map->scale
        };
    }
    else {
        result->normal_map = std::nullopt;
    }

    return result;
}

std::unique_ptr<::SpatialNode> GLTF::to_node() const {
    // Construct meshes.
    std::vector<std::shared_ptr<Mesh>> meshes;
    meshes.reserve(this->meshes.size());
    for (auto& cur_mesh_params : this->meshes) {
        meshes.push_back(construct_mesh(cur_mesh_params));
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

    // Pack this into a ConstructionEnvironment.
    ConstructionEnvironment constr_env {
        *this,
        meshes,
        materials,
        {}
    };

    // Convert glTF to node.
    if (this->nodes.size() == 1) {
        return ::to_node(constr_env, this->nodes[0]);
    }
    else if (this->nodes.size() > 1) {
        // We have multiple root nodes in this glTF, but
        // we must return only one root node.
        // So, create the spatial node with default
        // transform and make it root.
        auto result = std::make_unique<CompleteSpatialNode>(Transform());
        for (const auto& cur_gltf_node : this->nodes) {
            result->add_child(std::move(
                *::to_node(constr_env, cur_gltf_node).release()
            ));
        }
        return result;
    }
    else {
        // If there are 0 nodes, then just create a complete spatial node
        // with identity transform.
        return std::make_unique<CompleteSpatialNode>(Transform());
    }
}
