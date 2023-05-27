#pragma once

#include "GameSettings.hpp"

#include <memory>
#include <string_view>

namespace llengine {
class RenderingServer;
class BulletPhysicsServer;
class SpatialNode;

class GameInstance {
public:
    GameInstance(const GameSettings& settings);
    ~GameInstance();
    void start();

private:
    std::unique_ptr<RenderingServer> rendering_server;
    std::unique_ptr<BulletPhysicsServer> bullet_physics_server;
    std::unique_ptr<SpatialNode> root_node;
};
}