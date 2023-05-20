#pragma once

#include <vector> // std::vector
#include <string> // std::string

#include <nlohmann/json.hpp> // nlohmann::json

#include "SceneFile.hpp" // SceneFile
#include "nodes/SpatialNode.hpp" // SpatialNode

namespace llengine {
class SceneJSON : public SceneFile {
public:
    explicit SceneJSON(const std::string& json_path);
    std::unique_ptr<SpatialNode> to_node(EngineServers& servers) const override;

private:
    std::vector<std::unique_ptr<SceneFile>> scene_files;
    nlohmann::json json_map;

    std::unique_ptr<SpatialNode> to_node(
        EngineServers& servers, const nlohmann::json& json_node
    ) const;
};
}