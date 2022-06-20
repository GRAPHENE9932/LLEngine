#pragma once

#include <memory>
#include <vector>
#include <array>
#include <chrono>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objects/TexturedDrawableObject.hpp"
#include "objects/UnshadedDrawableObject.hpp"
#include "objects/ImageObject.hpp"
#include "objects/BitmapTextObject.hpp"
#include "objects/PointLight.hpp"
#include "objects/Camera.hpp"

class RenderingServer {
public:
    Camera* camera;
    std::array<PointLight, TX_DRW_POINT_LIGHTS_AMOUNT> point_lights;

    RenderingServer(int window_width, int window_height);
    ~RenderingServer();

    void set_update_callback(std::function<void(float)> func);
    GLFWwindow* get_window();

    void add_textured_drawable_object(std::shared_ptr<TexturedDrawableObject> obj, const bool overlay = false);
    void add_unshaded_drawable_object(std::shared_ptr<UnshadedDrawableObject> obj);
    void add_image_2d_object(std::shared_ptr<ImageObject> obj);
    void add_bitmap_text_object(std::shared_ptr<BitmapTextObject> obj, const bool overlay = false);

    void main_loop();

private:
    GLFWwindow* window;
    std::function<void(float)> update_callback;
    std::chrono::high_resolution_clock::time_point prev_frame_time;

    std::vector<std::shared_ptr<TexturedDrawableObject>> textured_objects;
    std::vector<std::shared_ptr<TexturedDrawableObject>> textured_objects_overlay;
    std::vector<std::shared_ptr<UnshadedDrawableObject>> unshaded_objects;
    std::vector<std::shared_ptr<ImageObject>> image_2d_objects;
    std::vector<std::shared_ptr<BitmapTextObject>> bitmap_text_objects;
    std::vector<std::shared_ptr<BitmapTextObject>> bitmap_text_objects_overlay;

    void init_window(int window_width, int window_height);
    void init_gl();
    void init_object_types();
};
