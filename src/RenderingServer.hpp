#pragma once

#include <memory>
#include <vector>
#include <array>
#include <chrono>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objects/DrawableObject.hpp"
#include "objects/PointLight.hpp"
#include "objects/Camera.hpp"

class RenderingServer {
public:
    Camera* camera;
    EnvironmentInfo env_info;

    RenderingServer(int window_width, int window_height);

    void set_update_callback(std::function<void(float)> func);
    GLFWwindow* get_window();

    void add_drawable_object(std::shared_ptr<DrawableObject> const obj, const bool overlay = false);

    void main_loop();

private:
    GLFWwindow* window;
    std::function<void(float)> update_callback;
    std::chrono::high_resolution_clock::time_point prev_frame_time;

    std::vector<std::shared_ptr<DrawableObject>> drawable_objects;
    std::vector<std::shared_ptr<DrawableObject>> drawable_objects_overlay;

    void init_window(int window_width, int window_height);
    void init_gl();
};
