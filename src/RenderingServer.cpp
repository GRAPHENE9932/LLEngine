#include <cstring>
#include <stdexcept>

#include <glm/ext/vector_int2.hpp>

#include "SceneTree.hpp"
#include "RenderingServer.hpp"
#include "nodes/core/rendering/ImageNode.hpp"
#include "nodes/core/rendering/BitmapTextNode.hpp"
#include "nodes/core/rendering/CommonDrawableNode.hpp"

RenderingServer::RenderingServer(SceneTree& scene_tree,
                                 glm::ivec2 window_extents) :
                                 scene_tree(scene_tree) {
    init_window(window_extents);
    init_gl();
}

RenderingServer::~RenderingServer() {
    Skybox::static_clean_up();

    glfwDestroyWindow(scene_tree.context.window);
    scene_tree.context.window = nullptr;
    glfwTerminate();
}

glm::ivec2 RenderingServer::get_window_extents() const {
    return window_extents;
}

void RenderingServer::set_skybox(const std::shared_ptr<Texture>& texture) {
    if (skybox == nullptr)
        skybox = std::make_unique<Skybox>(scene_tree, texture);
    else
        skybox->texture = texture;
}

void RenderingServer::init_window(glm::ivec2 window_extents) {
    int glfw_init_res = glfwInit();

    if (!glfw_init_res) {
        throw std::runtime_error(
            std::string("Failed to initialize GLFW. Error code: ") + std::to_string(glfw_init_res)
        );
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    scene_tree.context.window = glfwCreateWindow(window_extents.x, window_extents.y, "LLShooter",
            nullptr, nullptr);
    if (scene_tree.context.window == nullptr)
        throw std::runtime_error("Failed to create window.");
    this->window_extents = window_extents;
    scene_tree.inform_about_new_window_extents(window_extents);

    glfwMakeContextCurrent(scene_tree.context.window);
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
    glDepthFunc(GL_LEQUAL);

    // Enable gamma correction.
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Set blend function.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable backface culling.
    glEnable(GL_CULL_FACE);
}

void RenderingServer::main_loop() {
    Skybox::static_init();

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();
    do {
        // Measure time and callback.
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        scene_tree.context.delta_time =
                std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();
        scene_tree.invoke_update(*this);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Draw objects.
        const auto& drawables = scene_tree.get_drawables();
        for (const auto& cur_drawable : drawables)
            cur_drawable->draw();

        // Draw skybox.
        if (skybox != nullptr) {
            glDepthMask(GL_FALSE);
            skybox->draw();
            glDepthMask(GL_TRUE);
        }

        // Draw overlay objects.
        glClear(GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(scene_tree.context.window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(scene_tree.context.window));
}
