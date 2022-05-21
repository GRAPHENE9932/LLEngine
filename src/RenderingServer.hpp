#pragma once

#include <memory>
#include <vector>
#include <chrono>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objects/MonochromeDrawableObject.hpp"
#include "objects/TexturedDrawableObject.hpp"
#include "objects/Camera.hpp"

class RenderingServer {
public:
    Camera* camera;
    glm::vec3 light_direction;

    RenderingServer(int window_width, int window_height);
    ~RenderingServer();

    void set_update_callback(std::function<void(float)> func);
    GLFWwindow* get_window();
    void add_textured_drawable_object(TexturedDrawableObject* obj);

    void main_loop();

private:
    GLFWwindow* window;
    std::function<void(float)> update_callback;
    std::chrono::high_resolution_clock::time_point prev_frame_time;

    GLuint monochrome_program_id;
    GLuint mc_mvp_matrix_id;
    GLuint mc_color_id;
    GLuint mc_object_matrix_id;
    GLuint mc_camera_matrix_id;
    GLuint mc_light_direction_id;

    GLuint textured_program_id;
    GLuint tx_mvp_matrix_id;
    GLuint tx_object_matrix_id;
    GLuint tx_camera_matrix_id;
    GLuint tx_light_direction_id;

    std::vector<MonochromeDrawableObject*> monochrome_objects;
    std::vector<TexturedDrawableObject*> textured_objects;

    void init_window(int window_width, int window_height);
    void init_gl();
    void init_shaders();
};
