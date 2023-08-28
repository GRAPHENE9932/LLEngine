#pragma once

#include <set> // std::set
#include <memory> // std::shared_ptr
#include <chrono> // std::chrono::high_resolution_clock
#include <cstdint> // uint64_t
#include <optional> // std::optional
#include <functional>

#include <glm/vec2.hpp> // glm::ivec2
#include <glm/mat4x4.hpp> // glm::mat4

#include "QualitySettings.hpp"
#include "rendering/Window.hpp" // Window
#include "rendering/Skybox.hpp" // Skybox
#include "rendering/Texture.hpp"
#include "rendering/shaders/ShaderHolder.hpp"
#include "rendering/ManagedFramebufferID.hpp"

namespace llengine {
class Texture;
class RenderingNode;
class CameraNode;
class Drawable;
class GUICanvas;
struct PointLightNode;
struct SpotLight;

/**
 * @brief Stores non-owning pointers to all
 * rendering-related nodes and renders them.
 */
class RenderingServer {
public:
    explicit RenderingServer(glm::ivec2 window_size);
    RenderingServer(const RenderingServer& other) = delete;
    RenderingServer(RenderingServer&& other) = delete;
    ~RenderingServer() = default;

    RenderingServer& operator=(const RenderingServer& other) = delete;
    RenderingServer& operator=(RenderingServer&& other) = delete;

    void set_cubemap(const std::shared_ptr<Texture>& cubemap);
    void set_update_callback(const std::function<void(float)> callback) {
        this->update_callback = callback;
    }

    void main_loop();
    void stop();

    [[nodiscard]] bool is_mouse_button_pressed(std::uint8_t button) const;
    [[nodiscard]] glm::dvec2 get_cursor_position() const;
    void block_mouse_press();

    void apply_quality_settings(const QualitySettings& settings);

    /**
     * @brief Must be called by every Drawable upon its creation.
     *
     * Does nothing if drawable is nullptr.
     * @param drawable new freshly-created drawable.
     */
    void register_drawable(Drawable* drawable) noexcept;
    /**
     * @brief Must be called by every GUICanvas upon its creation.
     *
     * Does nothing if gui_canvas is nullptr.
     */
    void register_gui_canvas(GUICanvas* gui_canvas) noexcept;
    /**
     * @brief Must be called by every CameraNode upon its creation.
     * @param camera_node new freshly-created camera node.
     */
    void register_camera_node(CameraNode* camera_node) noexcept;
    /**
     * @brief Must be called by every PointLightNode upon its creation.
     * @param point_light new freshly-created point light.
     */
    void register_point_light(PointLightNode* point_light) noexcept;
    /**
     * @brief Must be called by every DrawableNode upon its destruction.
     *
     * Does nothing if argument is nullptr.
     * @param drawable_node drawable node that's going to be destroyed
     * or already destroyed.
     */
    void unregister_drawable_node(Drawable* drawable_node) noexcept;
    /**
     * @brief Must be called by every GUICanvas upon its destruction.
     *
     * Does nothing if argument is nullptr.
     * @param gui_canvas GUI canvas that's going to be destroyed
     * or already destroyed.
     */
    void unregister_gui_canvas(GUICanvas* gui_canvas) noexcept;
    /**
     * @brief Must be called by every PointLightNode upon its destruction.
     *
     * Does nothing if argument is nullptr.
     * @param point_light point light node that's going to be destroyed
     * or already destroyed.
     */
    void unregister_point_light(PointLightNode* point_light) noexcept;

    /**
     * @brief Should be called after every draw call to count triangles
     * that has been drawn.
     */
    void report_about_drawn_triangles(uint64_t triangles_amount) noexcept;

    /**
     * @brief Returns view matrix from the current camera.
     *
     * If camera is missing, returns identity matrix.
     */
    [[nodiscard]] glm::mat4 get_view_matrix() const noexcept;

    /**
     * @brief Returns position of the current camera.
     *
     * If camera is missing, returns (0.0, 0.0, 0.0).
     */
    [[nodiscard]] glm::vec3 get_camera_position() const noexcept;

    /**
     * @brief Returns projection matrix from the current camera.
     *
     * If camera is missing, returns identity matrix.
     */
    [[nodiscard]] glm::mat4 get_proj_matrix() const noexcept;

    /**
     * @brief Get the projection matrix multiplied by view matrix.
     *
     * Both these matrices can be retrieved from
     * get_view_matrix() and get_proj_matrix().
     */
    [[nodiscard]] glm::mat4 get_view_proj_matrix() const noexcept;
    [[nodiscard]] glm::mat4 get_dir_light_view_proj_matrix() const;

    void enable_shadow_mapping();
    void disable_shadow_mapping();
    [[nodiscard]] bool is_shadow_mapping_enabled() const;
    [[nodiscard]] TextureID get_shadow_map_texture_id() const;
    /**
     * @brief Get user-defined unadjusted shadow map bias
     *
     * Returns the shadow map bias that coincides with real shadow
     * map bias used in the shader at 45 degrees between the normal
     * and a light source with shadow map size of 1024x1024.
     */
    [[nodiscard]] float get_shadow_map_bias() const;
    [[nodiscard]] float get_adjusted_shadow_map_bias_at_45_deg() const;
    [[nodiscard]] glm::u32vec2 get_shadow_map_size() const;
    void set_shadow_map_size(glm::u32vec2 new_size);
    [[nodiscard]] glm::vec3 get_dir_light_direction() const;
    void set_shadow_drawing_distance(float new_distance);

    [[nodiscard]] const std::vector<PointLightNode*>& get_point_lights() const noexcept {
        return point_lights;
    }

    [[nodiscard]] const std::vector<Drawable*>& get_drawables() const noexcept {
        return drawables;
    }

    [[nodiscard]] const std::vector<GUICanvas*>& get_canvases() const noexcept {
        return gui_canvases;
    }

    [[nodiscard]] float get_delta_time() const {
        return delta_time;
    }

    [[nodiscard]] Window& get_window() {
        return window;
    }

    [[nodiscard]] ShaderHolder& get_shader_holder() {
        return shader_holder;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const Texture>>
    get_environment_cubemap(const glm::vec3& obj_position);
    [[nodiscard]] bool has_environment_cubemap();

    [[nodiscard]] std::optional<std::reference_wrapper<const Texture>>
    get_irradiance_map(const glm::vec3& obj_position);

    [[nodiscard]] std::optional<std::reference_wrapper<const Texture>>
    get_prefiltered_specular_map(const glm::vec3& obj_position);

    [[nodiscard]] const Texture& get_brdf_integration_map();

private:
    Window window;
    std::function<void(float)> update_callback;

    bool mouse_button_blocked = false;
    bool stop_requested = false;

    // Time point of the last frame.
    std::chrono::high_resolution_clock::time_point prev_frame_time;
    float delta_time = 1.0f;
    glm::vec3 dir_light_direction {-0.577350f, -0.577350f, -0.577350f};

    bool shadow_mapping_enabled = true;
    ManagedFramebufferID shadow_map_framebuffer;
    ManagedTextureID shadow_map_texture_id = 0;
    float shadow_map_bias_at_45_deg = 0.0008f;
    glm::u32vec2 shadow_map_size { 1024, 1024 };
    float shadow_map_drawing_distance = 20.0f;

    // Non-owning pointer to the current camera node.
    CameraNode* camera = nullptr;
    std::unique_ptr<Skybox> skybox = nullptr;
    std::optional<Texture> irradiance_map = std::nullopt;
    std::optional<Texture> prefiltered_specular_map = std::nullopt;
    std::optional<Texture> brdf_integration_map = std::nullopt;
    std::vector<Drawable*> drawables;
    std::vector<GUICanvas*> gui_canvases;
    std::vector<PointLightNode*> point_lights;

    ShaderHolder shader_holder;

    void unblock_mouse_press();
    void draw_non_overlay_objects();

    void initialize_shadow_map();
    void update_shadow_map();
    void delete_shadow_map();

    [[nodiscard]] std::array<glm::vec4, 8> get_camera_frustrum_corners(float max_distance) const;
};
}