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
}

RenderingServer::~RenderingServer() {
    Skybox::static_clean_up();
}

void RenderingServer::set_skybox(const std::shared_ptr<Texture>& texture) {
    if (skybox == nullptr)
        skybox = std::make_unique<Skybox>(scene_tree, texture);
    else
        skybox->texture = texture;
}

void RenderingServer::init_window(glm::ivec2 window_extents) {
    scene_tree.context.window.initialize(window_extents, "LLShooter", 3, 3);
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
        scene_tree.invoke_update();

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

        glfwSwapBuffers(scene_tree.context.window.get());
        glfwPollEvents();
    } while (!glfwWindowShouldClose(scene_tree.context.window.get()));
}
