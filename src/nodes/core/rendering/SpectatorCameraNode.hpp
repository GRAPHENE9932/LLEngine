#pragma once

#include <GLFW/glfw3.h> // Macros.

#include "nodes/core/rendering/CameraNode.hpp" // CameraNode

class SpectatorCameraNode : public CameraNode {
public:
    static constexpr int FORWARD_KEY = GLFW_KEY_W;
    static constexpr int BACKWARD_KEY = GLFW_KEY_S;
    static constexpr int RIGHT_KEY = GLFW_KEY_D;
    static constexpr int LEFT_KEY = GLFW_KEY_A;

    float sensivity = 0.001f;
    float speed = 1.0f;

    float x_angle = 0.0f;
    float y_angle = 0.0f;

    SpectatorCameraNode(const SpatialNode::SpatialParams& p, SceneTree& scene_tree);

    void update() override;
private:

    void update_rotation();
    void update_position();
};
