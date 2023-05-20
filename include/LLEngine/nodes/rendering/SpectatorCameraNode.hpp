#pragma once

#include <GLFW/glfw3.h> // Macros.

#include "nodes/rendering/CameraNode.hpp" // CameraNode
#include "rendering/Window.hpp" // Key

namespace llengine {
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

    SpectatorCameraNode(
        RenderingServer& rs, float display_ratio,
        float fov = glm::radians(90.0f), const Transform& transform = Transform()
    );

    void update() override;

private:
    void update_rotation();
    void update_position();
};
}