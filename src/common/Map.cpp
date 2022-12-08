#include <string> // std::string
#include <fstream> // std::ifstream
#include <cstdint> // uint32_t
#include <stdexcept> // std::runtime_error

#include <nlohmann/json.hpp> // nlohmann::json

#include "Map.hpp" // Map
#include "RenderingServer.hpp" // RenderingServer
#include "utils/json_conversion.hpp" // get_optional
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
    if (json_map.contains("scene_files")) {
        scene_files.reserve(json_map["scene_files"].size());
        for (const json& cur_file_path : json_map["scene_files"])
            scene_files.push_back(SceneFile::load_from_file(cur_file_path));
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

std::unique_ptr<PointLightNode> point_light_from_json(const json& root_json, RenderingServer& rs) {
    auto result = std::make_unique<PointLightNode>(
        root_json.get<SpatialNode::SpatialParams>(),
        rs
    );
    root_json.at("color").get_to(result->color);
    root_json.at("diffuse_strength").get_to(result->diffuse_strength);
    const auto& coef_json = root_json.at("coefficients");
    coef_json.at(0).get_to(result->const_coeff);
    coef_json.at(1).get_to(result->linear_coeff);
    coef_json.at(2).get_to(result->quadratic_coeff);

    return result;
}

std::unique_ptr<SpectatorCameraNode> player_to_node(const json& root_json, RenderingServer& rs) {
    const auto& extents = rs.get_window().get_window_size();
    const float aspect_ratio {static_cast<float>(extents.x) / extents.y};

    auto result = std::make_unique<SpectatorCameraNode>(
        rs, root_json.get<SpatialNode::SpatialParams>(), aspect_ratio,
        glm::radians(root_json.at("fov").get<float>())
    );
    return result;
}

std::unique_ptr<SpatialNode> Map::to_node(RenderingServer& rs, const json& json_node) const {
    std::unique_ptr<SpatialNode> result;
    std::string type = json_node.at("type").get<std::string>();
    if (type == "scene_file") {
        const SpatialNode::SpatialParams spat_params = json_node.get<SpatialNode::SpatialParams>();
        result = scene_files.at(json_node.at("scene_file_index").get<size_t>())->to_node(rs);
        result->set_translation(spat_params.translation + result->get_translation());
        result->set_scale(spat_params.scale * result->get_scale());
        result->set_rotation(spat_params.rotation * result->get_rotation());
    }
    else if (type == "empty") {
        const SpatialNode::SpatialParams spat_params = json_node.get<SpatialNode::SpatialParams>();
        result = std::make_unique<SpatialNode>(spat_params);
    }
    else if (type == "point_light") {
        result = point_light_from_json(json_node, rs);
    }
    else if (type == "player") {
        result = player_to_node(json_node, rs);
    }

    if (json_node.contains("children")) {
        for (const json& cur_json_child : json_node["children"]) {
            result->add_child(
                std::move(*to_node(rs, cur_json_child).release())
            );
        }
    }

    return result;
}

std::unique_ptr<SpatialNode> Map::to_node(RenderingServer& rs) const {
    return to_node(rs, json_map.at("root_node"));
}
