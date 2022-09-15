#pragma once

#include <vector> // std::vector
#include <memory> // std::shared_ptr, std::unique_ptr
#include <string> // std::string
#include <cstdint> // uint16_t, uint32_t
#include <variant> // std::variant, std::monostate
#include <optional> // std::optional
#include <string_view> // std::string_view

#include <GL/glew.h> // GLenum

#include "SceneTree.hpp" // SceneTree
#include "common/Mesh.hpp" // Mesh::Data
#include "common/Texture.hpp" // Texture::Parameters
#include "nodes/core/SpatialNode.hpp" // SpatialNode, SpatialNode::SpatialParams

class GLTF {
public:
    struct Node {
        std::string name;
        std::vector<GLTF::Node> children;
        SpatialNode::SpatialParams spatial_params;
        std::optional<uint32_t> mesh_index;
    };
    struct MeshParameters {
        std::variant<std::vector<uint16_t>, std::vector<uint32_t>, std::monostate> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec4> tangents;

        uint32_t material_index;
    };

    std::vector<MeshParameters> meshes;
    std::vector<Texture::Parameters> textures;
    std::vector<BasicMaterial<uint32_t>> materials;
    std::vector<GLTF::Node> nodes;

    explicit GLTF(std::string_view file_path);

    std::unique_ptr<::SpatialNode> to_node(SceneTree& scene_tree) const;
};
