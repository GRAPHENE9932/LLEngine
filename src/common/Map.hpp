#pragma once

#include <vector> // std::vector
#include <string> // std::string

#include <nlohmann/json.hpp> // nlohmann::json

#include "common/core/SceneFile.hpp" // SceneFile
#include "nodes/core/SpatialNode.hpp" // SpatialNode

class Map : public SceneFile {
public:
    explicit Map(const std::string& json_path);
    std::unique_ptr<SpatialNode> to_node(EngineServers& servers) const override;

private:
    std::vector<std::unique_ptr<SceneFile>> scene_files;
    nlohmann::json json_map;

    std::unique_ptr<SpatialNode> to_node(
        EngineServers& servers, const nlohmann::json& json_node
    ) const;
};
