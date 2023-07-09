#include "SceneJSON.hpp"
#include "nodes/gui/GUICanvas.hpp"
#include "utils/json_conversion.hpp"
#include "node_registration.hpp"

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <map>
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <utility>
#include <string_view>

using namespace llengine;

constexpr std::uint64_t CURRENT_VERSION = 1;

NodeProperty node_property_from_json(std::string_view key, const nlohmann::json& value) {
    if (value.is_number_integer()) {
        return NodeProperty(key, value.get<std::int64_t>());
    }
    else if (value.is_number_float()) {
        return NodeProperty(key, value.get<float>());
    }
    else if (value.is_boolean()) {
        return NodeProperty(key, value.get<bool>());
    }
    else if (value.is_string()) {
        return NodeProperty(key, value.get<std::string>());
    }
    else if (value.is_object()) {
        std::vector<NodeProperty> sub_properties;
        for (const auto& element : value.items()) {
            sub_properties.push_back(node_property_from_json(element.key(), element.value()));
        }
        return NodeProperty(key, std::move(sub_properties));
    }
    else if (value.is_array()) {
        if (value.size() == 0 || value[0].is_number_integer()) {
            return NodeProperty(key, value.get<std::vector<std::int64_t>>());
        }
        else if (value[0].is_number_float()) {
            return NodeProperty(key, value.get<std::vector<float>>());
        }
        else if (value[0].is_string()) {
            return NodeProperty(key, value.get<std::vector<std::string>>());
        }
        else if (value[0].is_array()) {
            if (value[0].size() == 2) {
                if (value[0][0].is_number_integer()) {
                    return NodeProperty(key, value.get<std::vector<glm::i32vec2>>());
                }
                else if (value[0][0].is_number_float()) {
                    return NodeProperty(key, value.get<std::vector<glm::vec2>>());
                }
                else {
                    throw std::runtime_error("Invalid vec2 type.");
                }
            }
            else if (value[0].size() == 3) {
                return NodeProperty(key, value.get<std::vector<glm::vec3>>());
            }
            else if (value[0].size() == 4) {
                return NodeProperty(key, value.get<std::vector<glm::vec4>>());
            }
            else {
                throw std::runtime_error("Invalid subvector size.");
            }
        }
        else {
            throw std::runtime_error("Unknown element type of the JSON array property.");
        }
    }
    else {
        throw std::runtime_error("Unknown JSON property type.");
    }
}

std::map<std::uint64_t, std::pair<SceneJSON::NodeData, std::optional<std::uint64_t>>>
create_nodes_map(const nlohmann::json& json) {
    std::map<std::uint64_t, std::pair<SceneJSON::NodeData, std::optional<std::uint64_t>>> nodes;

    for (const nlohmann::json& node_json : json) {
        std::uint64_t id = node_json.at("id");
        std::optional<std::uint64_t> parent_id = get_optional<std::uint64_t>(node_json, "parent_id");
        std::string type = node_json.at("type");

        std::vector<NodeProperty> properties;
        properties.reserve(node_json.size() - 3);
        for (const auto& element : node_json.items()) {
            if (element.key() == "id" || element.key() == "parent_id" || element.key() == "type") {
                continue;
            }

            try {
                properties.emplace_back(node_property_from_json(element.key(), element.value()));
            }
            catch (const std::exception& e) {
                throw std::runtime_error(fmt::format(
                    "Failed to read a node property from a JSON piece because: \"{}\".",
                    e.what()
                ));
            }
            catch (...) {
                throw std::runtime_error("Failed to read a node property from a JSON piece.");
            }
        }

        nodes[id] = {{type, std::move(properties), {}}, parent_id};
    }

    return nodes;
}

SceneJSON::NodeData initialize_root_node_data(const nlohmann::json& json) {
    auto nodes = create_nodes_map(json);

    std::size_t roots_count = std::count_if(nodes.begin(), nodes.end(), [] (const auto& pair) {
        return !pair.second.second.has_value();
    });
    if (roots_count != 1) {
        throw std::runtime_error(fmt::format(
            "There must be one node root, but we have {} of them.", roots_count
        ));
    }
    auto root_node_iter = std::find_if(nodes.begin(), nodes.end(), [] (const auto& pair) -> bool {
        return !pair.second.second.has_value();
    });

    for (auto& pair : nodes) {
        if (pair.first == root_node_iter->first) {
            continue;
        };
        nodes[*pair.second.second].first.children.emplace_back(std::move(pair.second.first));
    }

    return std::move(root_node_iter->second.first);
}

SceneJSON::SceneJSON(std::string_view json_path) {
    std::ifstream json_stream;
    json_stream.exceptions(std::ios::failbit);
    json_stream.open(std::string(json_path));
    nlohmann::json root_json = nlohmann::json::parse(json_stream);

    std::uint64_t scene_version = root_json.at("version").get<std::uint64_t>();
    if (scene_version != CURRENT_VERSION) {
        throw std::runtime_error(fmt::format(
            "Unsupported JSON scene version: {}. Only version {} is supported.",
            scene_version, CURRENT_VERSION
        ));
    }

    name = root_json.at("name");

    root_node_data = initialize_root_node_data(root_json.at("nodes"));
}

std::unique_ptr<Node> to_node(const SceneJSON::NodeData& data) {
    std::unique_ptr<Node> result = nullptr;

    if (data.type == "scene_file") {
        auto prop_iter = std::find_if(
            data.properties.begin(), data.properties.end(),
            [] (const NodeProperty& prop) -> bool {
                return prop.get_name() == "scene_file_path";
            }
        );
        if (prop_iter == data.properties.end()) {
            throw std::runtime_error("scene_file_path is not specified for a scene_file node.");
        }
        std::string scene_file_path = prop_iter->get<std::string>();

        result = SceneFile::load_from_file(scene_file_path)->to_node();

    }
    else {
        result = construct_node(data.type, data.properties);
    }
    
    for (const auto& child_data : data.children) {
        auto child_ptr = to_node(child_data);
        result->add_child(std::move(child_ptr));
    }
    return std::move(result);
}

std::unique_ptr<Node> SceneJSON::to_node() const {
    return std::move(::to_node(root_node_data));
}