#pragma once

#include <nodes/rendering/SpectatorCameraNode.hpp>

class MainCamera : public llengine::SpectatorCameraNode {
public:
    void start() final override;
    void update() final override;
    void on_keyboard_key_press(llengine::Key key) final override;

private:
    std::unique_ptr<llengine::SpatialNode> bullet = nullptr;
    bool clicked_before = false;

    void spawn_barrel();
};