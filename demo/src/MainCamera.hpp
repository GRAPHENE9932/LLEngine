#pragma once

#include <nodes/rendering/SpectatorCameraNode.hpp>

class MainCamera : public llengine::SpectatorCameraNode {
public:
    void start() final override;
    void update() final override;

private:
    std::unique_ptr<llengine::SpatialNode> barrel = nullptr;
    bool clicked_before = false;

    void spawn_barrel() const;
};