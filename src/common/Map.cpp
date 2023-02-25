#include <string> // std::string
#include <fstream> // std::ifstream
#include <cstdint> // uint32_t
#include <stdexcept> // std::runtime_error

#include <nlohmann/json.hpp> // nlohmann::json

#include "Map.hpp" // Map
#include "RenderingServer.hpp" // RenderingServer
#include "utils/json_conversion.hpp" // get_optional
#include "nodes/core/CompleteSpatialNode.hpp"
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

void from_json(const json& root_json, Transform& spat_params) {
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

std::unique_ptr<PointLightNode> point_light_from_json(const json& root_json) {
    auto result = std::make_unique<PointLightNode>(root_json.get<Transform>());
    root_json.at("color").get_to(result->color);
    root_json.at("diffuse_strength").get_to(result->diffuse_strength);
    const auto& coef_json = root_json.at("coefficients");
    coef_json.at(0).get_to(result->const_coeff);
    coef_json.at(1).get_to(result->linear_coeff);
    coef_json.at(2).get_to(result->quadratic_coeff);

    return result;
}

std::unique_ptr<SpectatorCameraNode> player_to_node(const json& root_json) {
    const auto& extents = RenderingServer::get_instance().get_window().get_window_size();
    const float aspect_ratio {static_cast<float>(extents.x) / extents.y};

    auto result = std::make_unique<SpectatorCameraNode>(
        root_json.get<Transform>(), aspect_ratio,
        glm::radians(root_json.at("fov").get<float>())
    );
    return result;
}

std::unique_ptr<SpatialNode> Map::to_node(const json& json_node) const {
    std::unique_ptr<SpatialNode> result;
    std::string type = json_node.at("type").get<std::string>();
    if (type == "scene_file") {
        const Transform spat_params = json_node.get<Transform>();
        result = scene_files.at(json_node.at("scene_file_index").get<size_t>())->to_node();
        result->set_translation(spat_params.translation + result->get_translation());
        result->set_scale(spat_params.scale * result->get_scale());
        result->set_rotation(spat_params.rotation * result->get_rotation());
    }
    else if (type == "empty") {
        const Transform spat_params = json_node.get<Transform>();
        result = std::make_unique<CompleteSpatialNode>(spat_params);
    }
    else if (type == "point_light") {
        result = point_light_from_json(json_node);
    }
    else if (type == "player") {
        result = player_to_node(json_node);
    }
    else {
        throw std::runtime_error("Unknown node type.");
    }

    if (json_node.contains("children")) {
        for (const json& cur_json_child : json_node["children"]) {
            result->add_child(
                to_node(cur_json_child)
            );
        }
    }

    return result;
}

std::unique_ptr<SpatialNode> Map::to_node() const {
    return to_node(json_map.at("root_node"));
}
