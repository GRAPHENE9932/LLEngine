#pragma once

#include "nodes/core/SpatialNode.hpp" // SpatialNode

#include <filesystem> // std::filesystem::path

class RenderingServer;

class SceneFile {
public:
    virtual ~SceneFile() = default;

    static std::unique_ptr<SceneFile> load_from_file(std::filesystem::path file_path);

    virtual std::unique_ptr<SpatialNode> to_node(RenderingServer& rs) const = 0;
};
