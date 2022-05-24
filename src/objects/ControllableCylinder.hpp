#pragma once

#include <GLFW/glfw3.h>
#include <glm/trigonometric.hpp>

#include "Camera.hpp"
#include "../structs/Cylinder.hpp"

class ControllableCylinder {
public:
    static constexpr float MAX_X_ROTATION = glm::radians(90.0f - 0.5f);
    static constexpr float MIN_X_ROTATION = glm::radians(-90.0f + 0.5f);

    static constexpr int FORWARD_KEY = GLFW_KEY_W;
    static constexpr int BACKWARD_KEY = GLFW_KEY_S;
    static constexpr int RIGHT_KEY = GLFW_KEY_D;
    static constexpr int LEFT_KEY = GLFW_KEY_A;
    static constexpr int JUMP_KEY = GLFW_KEY_SPACE;

    float mouse_sensivity;
    float forward_speed;
    float strafe_speed;
    float backward_speed;
    float jump_force;

    Cylinder cylinder;
    Camera* camera;
    GLFWwindow* window;

    ControllableCylinder(Camera* camera, GLFWwindow* window,
                         glm::vec3 position, float height, float radius);

    void update(float delta);
    void update_camera(float delta);
    void add_vertical_velocity(float difference);
    void zero_out_vertical_velocity();
    void landed();

private:
    /// Center coordinates in screen space.
    double center_x, center_y;

    float vertical_velocity = 0.0f;
    bool on_floor = true;

    void update_camera_orientation(float delta);
    void jump();
    glm::vec3 compute_right() const noexcept;
};
