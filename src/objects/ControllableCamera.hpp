#pragma once

#include <GLFW/glfw3.h>

#include "Camera.hpp"

class ControllableCamera : public Camera {
public:
    static const int FORWARD_KEY = GLFW_KEY_W;
    static const int BACKWARD_KEY = GLFW_KEY_S;
    static const int RIGHT_KEY = GLFW_KEY_D;
    static const int LEFT_KEY = GLFW_KEY_A;

    float mouse_sensivity;
    float forward_speed;
    float strafe_speed;
    float backward_speed;

    /// Center coordinates in screen space.
    double center_x, center_y;

    ControllableCamera(glm::vec3 position, float fov, int screen_size_x,
                       int screen_size_y, GLFWwindow* window);

    void update(float delta) noexcept;

private:
    double prev_mouse_x, prev_mouse_y;
    GLFWwindow* window;

    glm::vec3 compute_right() const noexcept;
};
