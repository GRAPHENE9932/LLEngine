#pragma once

#include <memory>
#include <vector>
#include <chrono>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objects/TexturedDrawableObject.hpp"
#include "objects/ImageObject2D.hpp"
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
    void add_image_2d_object(ImageObject2D* obj);

    void main_loop();

private:
    GLFWwindow* window;
    std::function<void(float)> update_callback;
    std::chrono::high_resolution_clock::time_point prev_frame_time;

    std::vector<TexturedDrawableObject*> textured_objects;
    std::vector<ImageObject2D*> image_2d_objects;

    void init_window(int window_width, int window_height);
    void init_gl();
    void init_object_types();
};
