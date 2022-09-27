#pragma once

#include <vector>
#include <string_view> // std::string_view

#include <nlohmann/json.hpp>

#include "SceneTree.hpp"
#include "common/core/GLTF.hpp" // GLTF
#include "nodes/core/SpatialNode.hpp" // SpatialNode

class Map {
public:
    explicit Map(const std::string& json_path);
    std::unique_ptr<SpatialNode> to_node(SceneTree& scene_tree) const;
    std::unique_ptr<SpatialNode> to_node(SceneTree& scene_tree, const nlohmann::json& json_node) const;

private:
    std::vector<GLTF> gltfs;
    nlohmann::json json_map;
};
