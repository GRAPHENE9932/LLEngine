#include "rendering/RenderingServer.hpp" // RenderingServer
#include "rendering/Skybox.hpp"
#include "rendering/GLFWWindow.hpp" // GLFWWindow
#include "logger.hpp"
#include "nodes/rendering/CameraNode.hpp"
#include "nodes/rendering/Drawable.hpp"
#include "nodes/gui/GUICanvas.hpp"
#include "MainFramebuffer.hpp"
#include "utils/texture_utils.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

using namespace llengine;

RenderingServer::RenderingServer(glm::ivec2 window_size, std::string_view window_title) :
    window(GLFWWindow(window_size, window_title, 3, 3)) {
    main_framebuffer = std::make_unique<MainFramebuffer>(window_size);
    current_default_framebuffer = main_framebuffer.get();
}

RenderingServer::~RenderingServer() {

}

void RenderingServer::set_cubemap(const std::shared_ptr<Texture>& cubemap) {
    if (cubemap->get_type() != Texture::Type::TEX_CUBEMAP) {
        throw std::invalid_argument("Specified cubemap texture for skybox is not actually a cubemap.");
    }

    this->skybox = std::make_unique<Skybox>(cubemap);
    this->global_lighting_environment.set_base_cubemap(cubemap);
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

        main_framebuffer->assign_framebuffer_size(get_window().get_framebuffer_size());
        glBindFramebuffer(GL_FRAMEBUFFER, main_framebuffer->get_framebuffer_id());
        glClear(GL_DEPTH_BUFFER_BIT);

        update_shadow_map();
        draw_non_overlay_objects();

        // Draw skybox.
        if (skybox != nullptr) {
            glDepthMask(GL_FALSE);
            skybox->draw(*this);
            glDepthMask(GL_TRUE);
        }

        // Draw overlay objects.
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (GUICanvas* canvas : gui_canvases) {
            canvas->draw();
        }
        glDisable(GL_BLEND);

        main_framebuffer->render_to_window(delta_time);

        window.swap_buffers();
        glfwPollEvents();
    } while (!window.window_should_close() && !stop_requested);
}

void RenderingServer::stop() {
    stop_requested = true;
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

void RenderingServer::apply_quality_settings(const QualitySettings& settings) {
    if (settings.shadow_mapping_enabled) {
        shadow_map = ShadowMap(settings.shadow_map_size);
        shadow_map->set_drawing_distance(settings.shadow_map_drawing_distance);
        if (camera != nullptr) {
            shadow_map->link_to_camera(get_current_camera_node());
        }
    }
    else {
        shadow_map = std::nullopt;
    }

    main_framebuffer->apply_postprocessing_settings(settings);
}

void RenderingServer::enable_face_culling() {
    face_culling_enabled = true;
    glCullFace(GL_TRUE);
}

void RenderingServer::disable_face_culling() {
    face_culling_enabled = false;
    glCullFace(GL_FALSE);
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

    if (shadow_map.has_value()) {
        shadow_map->link_to_camera(*camera_node);
    }
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

[[nodiscard]] CameraNode& RenderingServer::get_current_camera_node() {
    if (camera == nullptr) {
        throw std::runtime_error("There is no current camera node.");
    }

    return *camera;
}

[[nodiscard]] const CameraNode& RenderingServer::get_current_camera_node() const {
    if (camera == nullptr) {
        throw std::runtime_error("There is no current camera node.");
    }

    return *camera;
}

[[nodiscard]] bool RenderingServer::is_shadow_mapping_enabled() const {
    return shadow_map.has_value();
}

[[nodiscard]] ShadowMap& RenderingServer::get_shadow_map() {
    if (!shadow_map.has_value()) {
        throw std::runtime_error("Can not get shadow map as shadow mapping is disabled.");
    }

    return *shadow_map;
}

[[nodiscard]] float RenderingServer::get_exposure() const {
    return main_framebuffer->get_exposure();
}

const Texture& RenderingServer::get_brdf_integration_map() {
    if (!brdf_integration_map) {
        logger::info("Starting computing the BRDF integration map.");
        brdf_integration_map = tex_utils::compute_brdf_integration_map();
        logger::info("Finished computing the BRDF integration map.");
    }
    return *brdf_integration_map;
}

[[nodiscard]] FramebufferID RenderingServer::get_current_default_framebuffer_id() {
    assert(current_default_framebuffer != nullptr);
    return current_default_framebuffer->get_framebuffer_id();
}

void RenderingServer::draw_non_overlay_objects() {
    const Frustum frustum = get_current_camera_node().get_frustum();

    for (const auto& cur_drawable : get_drawables()) {
        if (cur_drawable->is_enabled() && !cur_drawable->is_outside_the_frustum(frustum)) {
            cur_drawable->draw();
        }
    }
}

void RenderingServer::update_shadow_map() {
    if (!is_shadow_mapping_enabled()) {
        return;
    }

    shadow_map->prepare_for_drawing();
    for (const auto& cur_drawable : get_drawables()) {
        if (cur_drawable->is_enabled()) {
            cur_drawable->draw_to_shadow_map();
        }
    }
    shadow_map->finish_drawing(
        face_culling_enabled, get_current_default_framebuffer_id(), get_window().get_framebuffer_size()
    );
}