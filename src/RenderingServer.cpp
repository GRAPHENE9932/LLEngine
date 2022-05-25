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

void RenderingServer::add_textured_drawable_object(TexturedDrawableObject* obj) {
    textured_objects.push_back(obj);
}

void RenderingServer::add_image_2d_object(ImageObject2D* obj) {
    image_2d_objects.push_back(obj);
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

    ImageObject2D::pre_init();
}

void RenderingServer::init_object_types() {
    TexturedDrawableObject::pre_init();
    ImageObject2D::pre_init();
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

        glm::mat4 view_matrix;
        auto camera_mvp = camera->compute_mvp_matrix(view_matrix);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw textured objects.
        glUseProgram(TexturedDrawableObject::program_id);
        for (int i = 0; i < textured_objects.size(); i++)
            textured_objects[i]->draw(&camera_mvp[0][0], &view_matrix[0][0], &light_direction[0]);

        // Draw Image2D objects.
        glUseProgram(ImageObject2D::program_id);
        for (int i = 0; i < image_2d_objects.size(); i++)
            image_2d_objects[i]->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));
}

RenderingServer::~RenderingServer() {
    for (DrawableObject* obj : textured_objects)
        delete obj;
    for (ImageObject2D* obj : image_2d_objects)
        delete obj;

    TexturedDrawableObject::clean_up();
    ImageObject2D::clean_up();
}
