#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objects/DrawableObject.hpp"
#include "objects/ControllableCamera.hpp"

class LLShooter {
public:
    void run_game();

    ~LLShooter();
private:
    GLFWwindow* window;
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    std::chrono::high_resolution_clock::time_point prev_frame_time;

    std::vector<DrawableObject*> drawable_objects;
    std::unique_ptr<ControllableCamera> camera;
    GLuint mvp_matrix_id;

    void init_window();

    void init_game();

    void main_loop();
};
