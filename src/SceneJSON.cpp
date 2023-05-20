#include <string> // std::string
#include <fstream> // std::ifstream
#include <cstdint> // uint32_t
#include <stdexcept> // std::runtime_error

#include <nlohmann/json.hpp> // nlohmann::json

#include "SceneJSON.hpp"
#include "rendering/RenderingServer.hpp" // RenderingServer
#include "utils/json_conversion.hpp" // get_optional
#include "nodes/CompleteSpatialNode.hpp"
#include "nodes/rendering/PointLightNode.hpp" // PointLightNode
#include "nodes/rendering/SpectatorCameraNode.hpp" // SpectatorCameraNode

using json = nlohmann::json;

using namespace llengine;

constexpr uint32_t CURRENT_MAP_VERSION = 1;

SceneJSON::SceneJSON(const std::string& json_path) {
    // Parse JSON.
    std::ifstream stream(json_path);
    json_map = json::parse(stream);

    // Check version.
    if (json_map.at("version") != CURRENT_MAP_VERSION)
        throw std::runtime_error("Invalid map version");

    // Load all external scene files.
    if (json_map.contains("scene_files")) {
        scene_files.reserve(json_map["scene_files"].size());
        for (const json& cur_file_path : json_map["scene_files"])
            scene_files.push_back(SceneFile::load_from_file(cur_file_path));
    }
}

namespace llengine {
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
}

std::unique_ptr<PointLightNode> point_light_from_json(RenderingServer& rs, const json& root_json) {
    auto result = std::make_unique<PointLightNode>(rs, root_json.get<Transform>());
    root_json.at("color").get_to(result->color);

    return result;
}

std::unique_ptr<SpectatorCameraNode> player_to_node(RenderingServer& rs, const json& root_json) {
    const auto& extents = rs.get_window().get_window_size();
    const float aspect_ratio {static_cast<float>(extents.x) / extents.y};

    auto result = std::make_unique<SpectatorCameraNode>(
        rs, aspect_ratio,
        glm::radians(root_json.at("fov").get<float>()),
        root_json.get<Transform>()
    );
    return result;
}

std::unique_ptr<SpatialNode> SceneJSON::to_node(EngineServers& servers, const json& json_node) const {
    std::unique_ptr<SpatialNode> result;
    std::string type = json_node.at("type").get<std::string>();
    if (type == "scene_file") {
        const Transform spat_params = json_node.get<Transform>();
        result = scene_files.at(json_node.at("scene_file_index").get<size_t>())->to_node(servers);
        result->set_translation(spat_params.translation + result->get_translation());
        result->set_scale(spat_params.scale * result->get_scale());
        result->set_rotation(spat_params.rotation * result->get_rotation());
    }
    else if (type == "empty") {
        const Transform spat_params = json_node.get<Transform>();
        result = std::make_unique<CompleteSpatialNode>(spat_params);
    }
    else if (type == "point_light") {
        result = point_light_from_json(servers.rs, json_node);
    }
    else if (type == "player") {
        result = player_to_node(servers.rs, json_node);
    }
    else {
        throw std::runtime_error("Unknown node type.");
    }

    if (json_node.contains("children")) {
        for (const json& cur_json_child : json_node["children"]) {
            result->add_child(
                to_node(servers, cur_json_child)
            );
        }
    }

    return result;
}

std::unique_ptr<SpatialNode> SceneJSON::to_node(EngineServers& servers) const {
    return to_node(servers, json_map.at("root_node"));
}
