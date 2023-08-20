#include "rendering/RenderingServer.hpp" // RenderingServer
#include "rendering/Skybox.hpp"
#include "rendering/GLFWWindow.hpp" // GLFWWindow
#include "logger.hpp"
#include "nodes/rendering/CameraNode.hpp"
#include "nodes/rendering/Drawable.hpp"
#include "nodes/gui/GUICanvas.hpp"
#include "utils/math.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

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
        for (GUICanvas* canvas : gui_canvases) {
            canvas->draw();
        }
        glDisable(GL_BLEND);

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
        enable_shadow_mapping();
    }
    else {
        disable_shadow_mapping();
    }

    set_shadow_map_size(settings.shadow_map_size);
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

[[nodiscard]] glm::mat4 RenderingServer::get_dir_light_view_proj_matrix() const {
    const glm::mat4 projection { glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 50.0f) };
    const glm::mat4 view { glm::lookAt(
        -dir_light_direction * 30.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    ) };

    return projection * view;
}

void RenderingServer::enable_shadow_mapping() {
    shadow_mapping_enabled = true;
}

void RenderingServer::disable_shadow_mapping() {
    shadow_mapping_enabled = false;
    delete_shadow_map();
}

[[nodiscard]] bool RenderingServer::is_shadow_mapping_enabled() const {
    return shadow_mapping_enabled;
}

[[nodiscard]] GLuint RenderingServer::get_shadow_map_texture_id() const {
    if (!is_shadow_mapping_enabled()) {
        throw std::runtime_error("Can't get shadow map texture ID, because shadows are disabled.");
    }

    if (shadow_map_texture_id == 0) {
        throw std::runtime_error("Can't get shadow map texture ID, bacause shadows map texture is uninitialized.");
    }

    return shadow_map_texture_id;
}

[[nodiscard]] float RenderingServer::get_shadow_map_bias() const {
    if (!is_shadow_mapping_enabled()) {
        throw std::runtime_error("Can't get shadow map bias, because shadows are disabled.");
    }

    return shadow_map_bias_at_45_deg;
}

[[nodiscard]] float RenderingServer::get_adjusted_shadow_map_bias_at_45_deg() const {
    float size_ratio = 1024.0f / shadow_map_size.x;
    float result = shadow_map_bias_at_45_deg * size_ratio;
    return result;
}

[[nodiscard]] glm::u32vec2 RenderingServer::get_shadow_map_size() const {
    return shadow_map_size;
}

void RenderingServer::set_shadow_map_size(glm::u32vec2 new_size) {
    if (new_size.x != new_size.y || !llengine::math_utils::is_power_of_two(new_size.x)) {
        throw std::runtime_error(
            "Invalid shadow map size specified. "
            "Shadow map must be square, bigger or equal to 4x4 and "
            "all extends must equal to a power of 2."
        );
    }

    shadow_map_size = new_size;
    initialize_shadow_map();
}

[[nodiscard]] glm::vec3 RenderingServer::get_dir_light_direction() const {
    if (!is_shadow_mapping_enabled()) {
        throw std::runtime_error("Can't get directional light direction, because shadows are disabled.");
    }

    return dir_light_direction;
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
        irradiance_map = env_cubemap->get().compute_irradiance_map();
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
        prefiltered_specular_map = env_cubemap->get().compute_prefiltered_specular_map();
        logger::info("Finished computing the prefiltered specular map.");
    }
    return *prefiltered_specular_map;
}

const Texture& RenderingServer::get_brdf_integration_map() {
    if (!brdf_integration_map) {
        logger::info("Starting computing the BRDF integration map.");
        brdf_integration_map = Texture::compute_brdf_integration_map();
        logger::info("Finished computing the BRDF integration map.");
    }
    return *brdf_integration_map;
}

bool RenderingServer::has_environment_cubemap() {
    return skybox != nullptr;
}

void RenderingServer::draw_non_overlay_objects() {
    for (const auto& cur_drawable : get_drawables()) {
        if (cur_drawable->is_enabled()) {
            cur_drawable->draw();
        }
    }
}

void RenderingServer::initialize_shadow_map() {
    delete_shadow_map();

    glGenFramebuffers(1, &shadow_map_framebuffer.get_ref());
    glGenTextures(1, &shadow_map_texture_id.get_ref());
    glBindTexture(GL_TEXTURE_2D, shadow_map_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_size.x, shadow_map_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_texture_id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void RenderingServer::update_shadow_map() {
    if (!is_shadow_mapping_enabled()) {
        return;
    }

    if (shadow_map_framebuffer == 0) {
        initialize_shadow_map();
    }

    glViewport(0, 0, shadow_map_size.x, shadow_map_size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    for (const auto& cur_drawable : get_drawables()) {
        if (cur_drawable->is_enabled()) {
            cur_drawable->draw_to_shadow_map();
        }
    }
    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    const auto window_size = get_window().get_window_size();
    glViewport(0, 0, window_size.x, window_size.y);
}

void RenderingServer::delete_shadow_map() {
    shadow_map_texture_id.delete_texture();
    shadow_map_framebuffer.delete_framebuffer();
}