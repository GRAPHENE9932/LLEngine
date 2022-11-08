#pragma once

#include <vector> // std::vector
#include <string> // std::string

#include <nlohmann/json.hpp> // nlohmann::json

#include "common/core/GLTF.hpp" // GLTF
#include "nodes/core/SpatialNode.hpp" // SpatialNode

class Map {
public:
    explicit Map(const std::string& json_path);
    std::unique_ptr<SpatialNode> to_node(RenderingServer& rs) const;
    std::unique_ptr<SpatialNode> to_node(RenderingServer& rs, const nlohmann::json& json_node) const;

private:
    std::vector<GLTF> gltfs;
    nlohmann::json json_map;
};
