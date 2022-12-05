#pragma once

#include <GLFW/glfw3.h> // Macros.

#include "nodes/core/rendering/CameraNode.hpp" // CameraNode
#include "common/core/IWindow.hpp" // Key

class SpectatorCameraNode : public CameraNode {
public:
    static constexpr Key FORWARD_KEY = Key::W;
    static constexpr Key BACKWARD_KEY = Key::S;
    static constexpr Key RIGHT_KEY = Key::D;
    static constexpr Key LEFT_KEY = Key::A;

    float sensivity = 0.001f;
    float speed = 1.0f;

    float x_angle = 0.0f;
    float y_angle = 0.0f;

    SpectatorCameraNode(RenderingServer& rs, const SpatialNode::SpatialParams& p,
        float display_ratio, float fov = glm::radians(90.0f));

    void update() override;

private:
    RenderingServer& rendering_server;

    void update_rotation();
    void update_position();
};
