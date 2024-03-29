#pragma once

#include "nodes/Node.hpp"

#include <memory>

namespace llengine {
class CustomNodeType;

class SceneFile {
public:
    virtual ~SceneFile() = default;

    static std::unique_ptr<SceneFile> load_from_file(std::string_view file_path);

    [[nodiscard]] virtual std::unique_ptr<Node> to_node(
        const std::vector<NodeProperty>& properties = {},
        const CustomNodeType* node_type = nullptr
    ) const = 0;
};
}