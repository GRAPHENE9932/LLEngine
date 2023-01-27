#include "RenderingServer.hpp" // RenderingServer
#include "BulletPhysicsServer.hpp"
#include "common/core/Skybox.hpp" // Skybox
#include "common/core/GLFWWindow.hpp" // GLFWWindow
#include "nodes/core/rendering/CameraNode.hpp"
#include "nodes/core/rendering/DrawableNode.hpp" // DrawableNode

#include <GLFW/glfw3.h>

RenderingServer::RenderingServer(glm::ivec2 window_extents) :
    window(*new GLFWWindow(window_extents, "LLShooter", 3, 3)),
    shader_manager(*this) {}

RenderingServer::~RenderingServer() {
    Skybox::static_clean_up();
}

void RenderingServer::set_skybox(const std::shared_ptr<Texture>& texture) {
    if (skybox == nullptr)
        skybox = std::make_unique<Skybox>(*this, texture);
    else
        skybox->texture = texture;
}

void RenderingServer::set_root_node(SpatialNode* root_node) {
    this->root_node = root_node;
}

void RenderingServer::main_loop() {
    Skybox::static_init();

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();
    do {
        // Measure delta time
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();

        // Do a physics step.
        BulletPhysicsServer::do_step_if_needed(delta_time);

        // Invoke update.
        if (root_node) {
            root_node->update();
        }

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Draw objects.
        for (const auto& cur_drawable : get_drawable_nodes())
            cur_drawable->draw();

        // Draw skybox.
        if (skybox != nullptr) {
            glDepthMask(GL_FALSE);
            skybox->draw();
            glDepthMask(GL_TRUE);
        }

        // Draw overlay objects.
        glClear(GL_DEPTH_BUFFER_BIT);

        window.swap_buffers();
        glfwPollEvents();
    } while (!window.window_should_close());
}

void RenderingServer::register_drawable_node(DrawableNode* drawable_node) noexcept {
    if (drawable_node) {
        drawable_nodes.insert(drawable_node);
    }
}

void RenderingServer::register_camera_node(CameraNode* camera_node) noexcept {
    camera = camera_node;
}

void RenderingServer::register_point_light(PointLightNode* point_light) noexcept {
    if (point_light) {
        point_lights.insert(point_light);
    }
}

void RenderingServer::unregister_drawable_node(DrawableNode* drawable_node) noexcept {
    drawable_nodes.erase(drawable_node);
}

void RenderingServer::unregister_point_light(PointLightNode* point_light) noexcept {
    point_lights.erase(point_light);
}

void RenderingServer::report_about_drawn_triangles(uint64_t triangles_amount) noexcept {
    // Does nothing now.
}

glm::mat4 RenderingServer::get_view_matrix() const noexcept {
    if (camera) {
        return camera->get_view_matrix();
    }
    else {
        return {};
    }
}

glm::mat4 RenderingServer::get_proj_matrix() const noexcept {
    if (camera) {
        return camera->get_proj_matrix();
    }
    else {
        return {};
    }
}

glm::mat4 RenderingServer::get_view_proj_matrix() const noexcept {
    return get_proj_matrix() * get_view_matrix();
}
