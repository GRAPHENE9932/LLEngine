#pragma once

#include "SceneFile.hpp"
#include "NodeProperty.hpp"
#include "nodes/RootNode.hpp"

#include <vector>

namespace llengine {
class SceneJSON : public SceneFile {
public:
    struct NodeData {
        std::string type;
        std::vector<NodeProperty> properties;
        std::vector<NodeData> children;
    };

    SceneJSON(std::string_view json_path);

    std::unique_ptr<Node> to_node(const std::vector<NodeProperty>& properties = {}) const override;

private:
    NodeData root_node_data;
    std::string name;
};
}