#pragma once

#include "nodes/core/SpatialNode.hpp" // SpatialNode

#include <filesystem> // std::filesystem::path

class RenderingServer;
class BulletPhysicsServer;

struct EngineServers {
    RenderingServer& rs;
    BulletPhysicsServer& bps;
};

class SceneFile {
public:
    virtual ~SceneFile() = default;

    static std::unique_ptr<SceneFile> load_from_file(const std::filesystem::path& file_path);

    virtual std::unique_ptr<SpatialNode> to_node(EngineServers& servers) const = 0;
};
