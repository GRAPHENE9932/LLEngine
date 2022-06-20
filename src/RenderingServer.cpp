#include <cstring>
#include <stdexcept>

#include "utils/utils.hpp"
#include "RenderingServer.hpp"

RenderingServer::RenderingServer(int window_width, int window_height) {
    init_window(window_width, window_height);
    init_gl();
    init_object_types();
}

void RenderingServer::set_update_callback(std::function<void(float)> func) {
    update_callback = func;
}

GLFWwindow* RenderingServer::get_window() {
    return window;
}

void RenderingServer::add_textured_drawable_object(std::shared_ptr<TexturedDrawableObject> obj, const bool overlay) {
    obj->camera = camera;
    obj->lights = &point_lights;

    if (overlay)
        textured_objects_overlay.push_back(obj);
    else
        textured_objects.push_back(obj);
}

void RenderingServer::add_unshaded_drawable_object(std::shared_ptr<UnshadedDrawableObject> obj) {
    unshaded_objects.push_back(obj);
}

void RenderingServer::add_image_2d_object(std::shared_ptr<ImageObject> obj) {
    image_2d_objects.push_back(obj);
}

void RenderingServer::add_bitmap_text_object(std::shared_ptr<BitmapTextObject> obj, const bool overlay) {
    if (overlay)
        bitmap_text_objects_overlay.push_back(obj);
    else
        bitmap_text_objects.push_back(obj);
}

void RenderingServer::init_window(int window_width, int window_height) {
    int glfw_init_res = glfwInit();

    if (!glfw_init_res) {
        throw std::runtime_error(
            std::string("Failed to initialize GLFW. Error code: ") + std::to_string(glfw_init_res)
        );
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(window_width, window_height, "LLShooter", nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("Failed to create window.");

    glfwMakeContextCurrent(window);
    //glfwSwapInterval(0);

    int glew_init_res = glewInit();
    if (glew_init_res != GLEW_OK) {
        throw std::runtime_error(
            std::string("Failed to initialize GLEW. Error code: " + std::to_string(glew_init_res))
        );
    }
}

void RenderingServer::init_gl() {
    // Create the VAO.
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set blend function.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable backface culling.
    glEnable(GL_CULL_FACE);
}

void RenderingServer::init_object_types() {
    TexturedDrawableObject::pre_init();
    UnshadedDrawableObject::pre_init();
    ImageObject::pre_init();
    BitmapTextObject::pre_init();
}

void RenderingServer::main_loop() {
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();
    do {
        // Measure time.
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        float delta = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();

        update_callback(delta);

        glm::mat4 view_matrix = camera->compute_view_matrix();
        glm::mat4 proj_matrix = camera->get_proj_matrix();
        glm::mat4 camera_vp = proj_matrix * view_matrix;

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Draw unshaded objects.
        glUseProgram(UnshadedDrawableObject::program_id);
        for (int i = 0; i < unshaded_objects.size(); i++)
            unshaded_objects[i]->draw(camera_vp);

        // Draw textured objects.
        glUseProgram(TexturedDrawableObject::program_id);
        for (int i = 0; i < textured_objects.size(); i++)
            textured_objects[i]->draw(camera_vp);
        
        // Draw bitmap text.
        glUseProgram(BitmapTextObject::get_program_id());
        for (int i = 0; i < bitmap_text_objects.size(); i++)
            bitmap_text_objects[i]->draw(camera_vp);

        // Overlay time.
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw textured objects.
        glUseProgram(TexturedDrawableObject::program_id);
        for (int i = 0; i < textured_objects_overlay.size(); i++)
            textured_objects_overlay[i]->draw(proj_matrix);

        // Draw Image2D objects.
        glUseProgram(ImageObject::program_id);
        for (int i = 0; i < image_2d_objects.size(); i++)
            image_2d_objects[i]->draw(proj_matrix);
        
        // Draw text.
        glUseProgram(BitmapTextObject::get_program_id());
        for (int i = 0; i < bitmap_text_objects_overlay.size(); i++)
            bitmap_text_objects_overlay[i]->draw(proj_matrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));
}

RenderingServer::~RenderingServer() {
    TexturedDrawableObject::clean_up();
    UnshadedDrawableObject::clean_up();
    ImageObject::clean_up();
    BitmapTextObject::clean_up();
}
