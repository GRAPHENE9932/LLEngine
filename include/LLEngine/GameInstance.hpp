#pragma once

#include "GameSettings.hpp"

#include <memory>

namespace llengine {
class RenderingServer;
class BulletPhysicsServer;
class RootNode;

class GameInstance {
public:
    GameInstance(const GameSettings& settings);
    ~GameInstance();
    void start();

    [[nodiscard]] RootNode& get_root_node() {
        return *root_node;
    }

private:
    std::unique_ptr<RenderingServer> rendering_server;
    std::unique_ptr<BulletPhysicsServer> bullet_physics_server;
    std::unique_ptr<RootNode> root_node;
};
}