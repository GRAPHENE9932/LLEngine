#pragma once

#include <vector> // std::vector
#include <memory> // std::shared_ptr, std::unique_ptr
#include <string> // std::string
#include <cstdint> // uint16_t, uint32_t
#include <variant> // std::variant, std::monostate
#include <optional> // std::optional
#include <string_view> // std::string_view

#include <nlohmann/json.hpp> // nlohmann::json

#include "Transform.hpp"
#include "rendering/Texture.hpp" // Texture::Parameters
#include "rendering/Material.hpp" // BasicMaterial
#include "SceneFile.hpp" // SceneFile

namespace llengine {
class GLTF : public SceneFile {
public:
    struct Node {
        Node(const GLTF& master, const nlohmann::json& gltf_json, const nlohmann::json& node_json);

        const GLTF& master;

        std::string name;
        std::optional<nlohmann::json> extras;
        std::vector<GLTF::Node> children;
        Transform transform;
        std::optional<uint32_t> mesh_index;

        [[nodiscard]] bool is_drawable() const;
        [[nodiscard]] bool is_rigid_body() const;
        [[nodiscard]] std::unique_ptr<Node> to_node() const;
    };
    struct MeshParameters {
        std::variant<std::vector<uint16_t>, std::vector<uint32_t>, std::monostate> indices;
        std::vector<glm::vec3> vertices;
        std::optional<std::vector<glm::vec2>> uvs;
        std::optional<std::vector<glm::vec3>> normals;
        std::optional<std::vector<glm::vec4>> tangents;

        uint32_t material_index;
    };

    std::vector<MeshParameters> meshes;
    std::vector<TexLoadingParams> textures;
    std::vector<BasicMaterial<uint32_t>> materials;
    std::vector<GLTF::Node> nodes;

    /**
    * @brief Loads and parses the glTF file.
    *
    * Some limitations:
    *  - KTX2 textures only.
    *  - Base64 data is not supported.
    *  - Remote (network) data is not supported.
    *  - Only KHR_texture_transform extension, KHR_texture_basisu and KHR_materials_emissive_strength are supported.
    *
    * Rigid bodies:
    *    You can make node a rigid body using custom properties (extras).
    *    If this node also have a mesh, then a new node with mesh will be
    *    created with rigid body parent. This game engine doesn't support
    *    nodes that are drawable and physical at the same time.
    *    Firstly, it must contain the "collider_type" string property
    *    that contains one of these string values:
    *      - "BOX": then it also must contain "extents" property of
    *        type float array. It must contain 3 values that
    *        represents width (x), height (y), length (z).
    *      - "SPHERE": then it also must contain "radius" property
    *        of type float.
    *      - "CYLINDER": the it also must contain "radius" and
    *        "height" properties.
    *
    *    Secondly, it may contain the "mass" property of type float.
    *    If mass is absent or equals to 0.0, then this rigid body
    *    will be considered as static.
    */
    explicit GLTF(std::string_view file_path);

    [[nodiscard]] std::unique_ptr<llengine::Node> to_node(
        const std::vector<NodeProperty>& properties = {},
        const CustomNodeType* node_type = nullptr
    ) const override;
};
}