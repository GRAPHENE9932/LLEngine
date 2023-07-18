#include "rendering/RenderingServer.hpp" // RenderingServer
#include "rendering/Skybox.hpp"
#include "rendering/GLFWWindow.hpp" // GLFWWindow
#include "logger.hpp"
#include "nodes/rendering/CameraNode.hpp"
#include "nodes/rendering/Drawable.hpp"
#include "nodes/gui/GUICanvas.hpp"

#include <GLFW/glfw3.h>

using namespace llengine;

RenderingServer::RenderingServer(glm::ivec2 window_size) :
    window(GLFWWindow(window_size, "LLShooter", 3, 3)) {
}

void RenderingServer::set_cubemap(const std::shared_ptr<Texture>& cubemap) {
    if (!cubemap->is_cubemap()) {
        throw std::invalid_argument("Specified cubemap texture for skybox is not actually a cubemap.");
    }

    this->skybox = std::make_unique<Skybox>(cubemap);
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

        unblock_mouse_press();

        // Invoke callback.
        update_callback(delta_time);
        
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Draw objects.
        for (const auto& cur_drawable : get_drawables())
            cur_drawable->draw();

        // Draw skybox.
        if (skybox != nullptr) {
            glDepthMask(GL_FALSE);
            skybox->draw(*this);
            glDepthMask(GL_TRUE);
        }

        // Draw overlay objects.
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        for (GUICanvas* canvas : gui_canvases) {
            canvas->draw();
        }
        glDisable(GL_BLEND);

        window.swap_buffers();
        glfwPollEvents();
    } while (!window.window_should_close());
}

[[nodiscard]] bool RenderingServer::is_mouse_button_pressed(std::uint8_t button) const {
    if (mouse_button_blocked) {
        return false;
    }

    return window.is_mouse_button_pressed(button);
}

[[nodiscard]] glm::dvec2 RenderingServer::get_cursor_position() const {
    return window.get_cursor_position();
}

void RenderingServer::block_mouse_press() {
    mouse_button_blocked = true;
}

void RenderingServer::unblock_mouse_press() {
    mouse_button_blocked = false;
}

void RenderingServer::register_drawable(Drawable* drawable) noexcept {
    if (drawable) {
        drawables.push_back(drawable);
    }
}

void RenderingServer::register_gui_canvas(GUICanvas* gui_node) noexcept {
    gui_canvases.push_back(gui_node);
}

void RenderingServer::register_camera_node(CameraNode* camera_node) noexcept {
    camera = camera_node;
}

void RenderingServer::register_point_light(PointLightNode* point_light) noexcept {
    if (point_light) {
        point_lights.push_back(point_light);
    }
}

void RenderingServer::unregister_drawable_node(Drawable* drawable_node) noexcept {
    const auto iter {
        std::find(drawables.begin(), drawables.end(), drawable_node)
    };
    if (iter != drawables.end()) {
        drawables.erase(iter);
    }
}

void RenderingServer::unregister_gui_canvas(GUICanvas* gui_canvas) noexcept {
    const auto iter {
        std::find(gui_canvases.begin(), gui_canvases.end(), gui_canvas)
    };
    if (iter != gui_canvases.end()) {
        gui_canvases.erase(iter);
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
        logger::info("Starting computing the irradiance map.");
        irradiance_map = env_cubemap->get().compute_irradiance_map(shader_holder.get_irradiance_precomputer_shader());
        logger::info("Finished computing the irradiance map.");
    }
    return *irradiance_map;
}

std::optional<std::reference_wrapper<const Texture>>
RenderingServer::get_prefiltered_specular_map(const glm::vec3& obj_position) {
    auto env_cubemap {get_environment_cubemap(obj_position)};

    if (!env_cubemap.has_value()) {
        return std::nullopt;
    }
    if (!prefiltered_specular_map) {
        logger::info("Starting computing the prefiltered specular map.");
        prefiltered_specular_map = env_cubemap->get().compute_prefiltered_specular_map(shader_holder.get_specular_prefilter_shader());
        logger::info("Finished computing the prefiltered specular map.");
    }
    return *prefiltered_specular_map;
}

const Texture& RenderingServer::get_brdf_integration_map() {
    if (!brdf_integration_map) {
        logger::info("Starting computing the BRDF integration map.");
        brdf_integration_map = Texture::compute_brdf_integration_map(shader_holder.get_brdf_integration_mapper_shader());
        logger::info("Finished computing the BRDF integration map.");
    }
    return *brdf_integration_map;
}

bool RenderingServer::have_environment_cubemap() {
    return skybox != nullptr;
}
