#include "rendering/RenderingServer.hpp" // RenderingServer
#include "rendering/Mesh.hpp"
#include "rendering/Skybox.hpp"
#include "rendering/GLFWWindow.hpp" // GLFWWindow
#include "logger.hpp"
#include "nodes/rendering/CameraNode.hpp"
#include "nodes/rendering/Drawable.hpp"
#include "nodes/gui/GUICanvas.hpp"
#include "utils/math.hpp"
#include "MainFramebuffer.hpp"

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
        cached_camera_frustum_corners_are_valid = false;

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
        enable_shadow_mapping();
    }
    else {
        disable_shadow_mapping();
    }

    set_shadow_map_size(settings.shadow_map_size);
    set_shadow_drawing_distance(settings.shadow_map_drawing_distance);
    main_framebuffer->apply_postprocessing_settings(settings);
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

[[nodiscard]] const std::array<glm::vec4, 8>& RenderingServer::get_camera_frustrum_corners(float max_distance) const {
    if (cached_camera_frustum_corners_are_valid) {
        return cached_camera_frustum_corners;
    }

    assert(camera != nullptr);

    const glm::mat4 proj_inverse = glm::inverse(get_proj_matrix());
    const glm::mat4 view_inverse = glm::inverse(get_view_matrix());
    const float far_corners_coeff = std::min(max_distance / camera->get_far_distance(), 1.0f);

    for (std::uint8_t corner_index = 0; corner_index < 8; corner_index++) {
        glm::vec4 corner {
            corner_index & 0x01 ? 1.0 : -1.0,
            corner_index & 0x02 ? 1.0 : -1.0,
            corner_index & 0x04 ? 1.0 : -1.0,
            1.0f
        };

        corner = proj_inverse * corner;
        if ((corner_index & 0x04)) {
            corner.x *= far_corners_coeff;
            corner.y *= far_corners_coeff;
            corner.z *= far_corners_coeff;
        }
        corner = view_inverse * corner;

        cached_camera_frustum_corners[corner_index] = corner / corner.w;
    }

    cached_camera_frustum_corners_are_valid = true;
    return cached_camera_frustum_corners;
}

[[nodiscard]] glm::vec4 compute_average_vector(const std::array<glm::vec4, 8>& vectors) {
    glm::vec4 result {0.0f, 0.0f, 0.0f, 0.0f};
    for (const glm::vec4 cur_vec : vectors) {
        result += cur_vec * (1.0f / 8.0f);
    }
    return result;
}

constexpr float SHADOW_CAMERA_CLEARANCE = 25.0f;

[[nodiscard]] glm::mat4 RenderingServer::get_dir_light_view_proj_matrix() const {
    const auto camera_frustrum_corners = get_camera_frustrum_corners(shadow_map_drawing_distance * 2.0f);
    const glm::vec3 camera_frustrum_center = glm::vec3(compute_average_vector(camera_frustrum_corners));
    glm::vec3 flat_frustrum_center {camera_frustrum_center.x, 0.0f, camera_frustrum_center.z};

    const glm::mat4 view { glm::lookAt(
        -dir_light_direction * SHADOW_CAMERA_CLEARANCE + flat_frustrum_center,
        flat_frustrum_center,
        {0.0f, 1.0f, 0.0f}
    ) };

    const glm::mat4 projection { glm::ortho(
        -shadow_map_drawing_distance,
        shadow_map_drawing_distance,
        -shadow_map_drawing_distance,
        shadow_map_drawing_distance,
        camera_frustrum_center.y - SHADOW_CAMERA_CLEARANCE * 2.0f,
        camera_frustrum_center.y + SHADOW_CAMERA_CLEARANCE * 2.0f
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

void RenderingServer::set_shadow_drawing_distance(float new_distance) {
    shadow_map_drawing_distance = new_distance;
}

[[nodiscard]] float RenderingServer::get_exposure() const {
    return main_framebuffer->get_exposure();
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

[[nodiscard]] FramebufferID RenderingServer::get_current_default_framebuffer_id() {
    assert(current_default_framebuffer != nullptr);
    return current_default_framebuffer->get_framebuffer_id();
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

constexpr std::array<float, 4> SHADOW_MAP_BORDER_COLOR {1.0f, 1.0f, 1.0f, 1.0f};

void RenderingServer::initialize_shadow_map() {
    delete_shadow_map();

    glGenFramebuffers(1, &shadow_map_framebuffer.get_ref());
    glGenTextures(1, &shadow_map_texture_id.get_ref());
    glBindTexture(GL_TEXTURE_2D, shadow_map_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_size.x, shadow_map_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, SHADOW_MAP_BORDER_COLOR.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
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
    glBindFramebuffer(GL_FRAMEBUFFER, get_current_default_framebuffer_id());
    const auto framebuffer_size = get_window().get_framebuffer_size();
    glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
}

void RenderingServer::delete_shadow_map() {
    shadow_map_texture_id.delete_texture();
    shadow_map_framebuffer.delete_framebuffer();
}