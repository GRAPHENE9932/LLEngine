#include "RenderingServer.hpp" // RenderingServer
#include "BulletPhysicsServer.hpp"
#include "common/core/Skybox.hpp"
#include "common/core/GLFWWindow.hpp" // GLFWWindow
#include "nodes/core/rendering/CameraNode.hpp"
#include "nodes/core/rendering/DrawableNode.hpp" // DrawableNode
#include "utils/texture_tools.hpp"

#include <GLFW/glfw3.h>

RenderingServer::RenderingServer(glm::ivec2 window_size) :
    window(*new GLFWWindow(window_size, "LLShooter", 3, 3)) {}

RenderingServer::~RenderingServer() {}

void RenderingServer::set_cubemap(const std::shared_ptr<Texture>& cubemap) {
    if (!cubemap->is_cubemap()) {
        throw std::invalid_argument("Specified cubemap texture for skybox is not actually a cubemap.");
    }

    this->skybox = std::make_unique<Skybox>(*this, cubemap);
}
void RenderingServer::main_loop() {
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();
    do {
        // Measure delta time
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();

        // Invoke callback.
        update_callback(delta_time);

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
        drawable_nodes.push_back(drawable_node);
    }
}

void RenderingServer::register_camera_node(CameraNode* camera_node) noexcept {
    camera = camera_node;
}

void RenderingServer::register_point_light(PointLightNode* point_light) noexcept {
    if (point_light) {
        point_lights.push_back(point_light);
    }
}

void RenderingServer::unregister_drawable_node(DrawableNode* drawable_node) noexcept {
    const auto iter {
        std::find(drawable_nodes.begin(), drawable_nodes.end(), drawable_node)
    };
    if (iter != drawable_nodes.end()) {
        drawable_nodes.erase(iter);
    }
}

void RenderingServer::unregister_point_light(PointLightNode* point_light) noexcept {
    const auto iter {
        std::find(point_lights.begin(), point_lights.end(), point_light)
    };
    if (iter != point_lights.end()) {
        point_lights.erase(iter);
    }
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

glm::vec3 RenderingServer::get_camera_position() const noexcept {
    if (camera) {
        return camera->get_global_position();
    }
    else {
        return {0.0f, 0.0f, 0.0f};
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

std::optional<std::reference_wrapper<const Texture>>
RenderingServer::get_environment_cubemap(const glm::vec3& camera_position) {
    if (skybox) {
        return skybox->get_texture();
    }
    else {
        return std::nullopt;
    }
}

std::optional<std::reference_wrapper<const Texture>>
RenderingServer::get_irradiance_map(const glm::vec3& obj_position) {
    auto env_cubemap {get_environment_cubemap(obj_position)};

    if (!env_cubemap.has_value()) {
        return std::nullopt;
    }
    if (!irradiance_map) {
        irradiance_map = compute_irradiance_map(env_cubemap->get(), shader_holder.get_irradiance_precomputer_shader());
    }
    return *irradiance_map;
}

bool RenderingServer::have_environment_cubemap() {
    return skybox != nullptr;
}
