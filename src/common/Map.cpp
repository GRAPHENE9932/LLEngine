#include <string> // std::string
#include <fstream> // std::ifstream
#include <cstdint> // uint32_t
#include <stdexcept> // std::runtime_error

#include <nlohmann/json.hpp> // nlohmann::json
#include <utility>

#include "Map.hpp" // Map
#include "SceneTree.hpp" // SceneTree
#include "utils/json_conversion.hpp"
#include "nodes/core/SpatialNode.hpp" // SpatialNode
#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode
#include "nodes/core/rendering/SpectatorCameraNode.hpp" // SpectatorCameraNode

using json = nlohmann::json;

constexpr uint32_t CURRENT_MAP_VERSION = 1;

Map::Map(const std::string& json_path) {
    // Parse JSON.
    std::ifstream stream(json_path);
    json_map = json::parse(stream);

    // Check version.
    if (json_map.at("version") != CURRENT_MAP_VERSION)
        throw std::runtime_error("Invalid map version");

    // Load all glTF files.
    if (json_map.contains("gltf_files")) {
        gltfs.reserve(json_map["gltf_files"].size());
        for (const std::string& cur_file_path : json_map["gltf_files"])
            gltfs.emplace_back(cur_file_path);
    }
}

void from_json(const json& root_json, SpatialNode::SpatialParams& spat_params) {
    const glm::quat testo({0.0f, 0.0f, 0.0f});
    spat_params = {
        get_optional<glm::vec3>(root_json, "position")
                .value_or(glm::vec3(0.0f, 0.0f, 0.0f)),
        get_optional<glm::vec3>(root_json, "scale")
                .value_or(glm::vec3(1.0f, 1.0f, 1.0f)),
        glm::quat(get_optional<glm::vec3>(root_json, "rotation")
                .value_or(glm::vec3(0.0f, 0.0f, 0.0f))),
    };
}

std::unique_ptr<PointLightNode> point_light_from_json(const json& root_json, SceneTree& scene_tree) {
    auto result = std::make_unique<PointLightNode>(
        root_json.get<SpatialNode::SpatialParams>(),
        scene_tree
    );
    root_json.at("color").get_to(result->color);
    root_json.at("diffuse_strength").get_to(result->diffuse_strength);
    const auto& coef_json = root_json.at("coefficients");
    coef_json.at(0).get_to(result->const_coeff);
    coef_json.at(1).get_to(result->linear_coeff);
    coef_json.at(2).get_to(result->quadratic_coeff);

    return result;
}

std::unique_ptr<SpectatorCameraNode> player_to_node(const json& root_json, SceneTree& scene_tree) {
    const auto& extents = scene_tree.get_context().window_extents;
    const float aspect_ratio {static_cast<float>(extents.x) / extents.y};

    auto result = std::make_unique<SpectatorCameraNode>(
        root_json.get<SpatialNode::SpatialParams>(),
        scene_tree
    );
    result->recompute_proj_matrix(
        glm::radians(root_json.at("fov").get<float>()),
        aspect_ratio
    );
    return result;
}

std::unique_ptr<SpatialNode> Map::to_node(SceneTree& scene_tree, const json& json_node) const {
    std::unique_ptr<SpatialNode> result;
    std::string type = json_node.at("type").get<std::string>();
    if (type == "gltf") {
        // TODO: use spatial params.
        const SpatialNode::SpatialParams spat_params = json_node.get<SpatialNode::SpatialParams>();
        result = gltfs.at(json_node.at("gltf_index").get<size_t>()).to_node(scene_tree);
    }
    else if (type == "empty") {
        const SpatialNode::SpatialParams spat_params = json_node.get<SpatialNode::SpatialParams>();
        result = std::make_unique<SpatialNode>(spat_params, scene_tree);
    }
    else if (type == "point_light") {
        result = point_light_from_json(json_node, scene_tree);
    }
    else if (type == "player") {
        result = player_to_node(json_node, scene_tree);
    }

    if (json_node.contains("children")) {
        for (const json& cur_json_child : json_node["children"]) {
            result->add_child(std::move(
                to_node(scene_tree, cur_json_child)
            ));
        }
    }

    return result;
}

std::unique_ptr<SpatialNode> Map::to_node(SceneTree& scene_tree) const {
    return to_node(scene_tree, json_map.at("root_node"));
}
