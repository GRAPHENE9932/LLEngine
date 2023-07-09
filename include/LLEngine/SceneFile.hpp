#pragma once

#include "nodes/Node.hpp"

namespace llengine {
class SceneFile {
public:
    virtual ~SceneFile() = default;

    static std::unique_ptr<SceneFile> load_from_file(std::string_view file_path);

    virtual std::unique_ptr<Node> to_node() const = 0;
};
}