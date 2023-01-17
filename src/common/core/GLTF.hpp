#pragma once

#include <vector> // std::vector
#include <memory> // std::shared_ptr, std::unique_ptr
#include <string> // std::string
#include <cstdint> // uint16_t, uint32_t
#include <variant> // std::variant, std::monostate
#include <optional> // std::optional
#include <string_view> // std::string_view
#include <unordered_map> // std::unordered_map

#include <GL/glew.h> // GLenum
#include <nlohmann/json.hpp> // nlohmann:json

#include "structs/Transform.hpp"
#include "common/core/Mesh.hpp" // Mesh::Data
#include "common/core/Texture.hpp" // Texture::Parameters
#include "common/core/Material.hpp" // BasicMaterial
#include "common/core/SceneFile.hpp" // SceneFile

class RenderingServer;

class GLTF : public SceneFile {
public:
    struct Node {
        std::string name;
        std::unordered_map<std::string, nlohmann::json> extras;
        std::vector<GLTF::Node> children;
        Transform transform;
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

    /**
    * @brief Loads and parses the glTF file.
    *
    * Some limitations:
    *  - KTX2 textures only.
    *  - Base64 data is not supported.
    *  - Remote (network) data is not supported.
    *  - Only KHR_texture_transform extension is supported.
    */
    explicit GLTF(std::string_view file_path);

    std::unique_ptr<::SpatialNode> to_node(RenderingServer& rs) const override;
};
