#pragma once

#include <memory> // std::shared_ptr
#include <chrono> // std::chrono::high_resolution_clock
#include <cstdint> // uint64_t
#include <optional> // std::optional
#include <functional>

#include <glm/vec2.hpp> // glm::ivec2
#include <glm/mat4x4.hpp> // glm::mat4

#include "QualitySettings.hpp"
#include "rendering/LightingEnvironment.hpp"
#include "rendering/ShadowMap.hpp"
#include "rendering/Window.hpp" // Window
#include "rendering/Skybox.hpp" // Skybox
#include "rendering/Texture.hpp"

namespace llengine {
class Texture;
class RenderingNode;
class CameraNode;
class Drawable;
class GUICanvas;
class MainFramebuffer;
struct PointLightNode;
struct SpotLight;

/**
 * @brief Stores non-owning pointers to all
 * rendering-related nodes and renders them.
 */
class RenderingServer {
public:
    explicit RenderingServer(glm::ivec2 window_size, std::string_view window_title = "LLEngine");
    RenderingServer(const RenderingServer& other) = delete;
    RenderingServer(RenderingServer&& other) = delete;
    ~RenderingServer();

    RenderingServer& operator=(const RenderingServer& other) = delete;
    RenderingServer& operator=(RenderingServer&& other) = delete;

    [[nodiscard]] static RenderingServer& current();
    [[nodiscard]] static RenderingServer* current_optional();
    [[nodiscard]] static std::uint32_t current_context_id();

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
    [[nodiscard]] const QualitySettings& get_quality_settings() const;
    void enable_face_culling();
    void disable_face_culling();

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
     * @brief Returns reference to the current camera node.
     * 
     * Throws if there is no current_camera.
     */
    [[nodiscard]] CameraNode& get_current_camera_node();
    /**
     * @brief Returns const reference to the current camera node.
     * 
     * Throws if there is no current_camera.
     */
    [[nodiscard]] const CameraNode& get_current_camera_node() const;

    [[nodiscard]] bool is_shadow_mapping_enabled() const;
    [[nodiscard]] ShadowMap& get_shadow_map();

    [[nodiscard]] float get_exposure() const;

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

    [[nodiscard]] LightingEnvironment& get_global_lighting_environment() {
        return global_lighting_environment;
    }

    [[nodiscard]] FramebufferID _get_main_framebuffer_id() const;

private:
    Window window;
    std::uint32_t context_id;
    QualitySettings quality_settings;
    std::function<void(float)> update_callback;

    bool mouse_button_blocked = false;
    bool stop_requested = false;

    // Time point of the last frame.
    std::chrono::high_resolution_clock::time_point prev_frame_time;
    float delta_time = 1.0f;

    std::unique_ptr<MainFramebuffer> main_framebuffer;

    std::optional<ShadowMap> shadow_map;
    bool face_culling_enabled = true;

    // Non-owning pointer to the current camera node.
    CameraNode* camera = nullptr;

    std::unique_ptr<Skybox> skybox = nullptr;
    LightingEnvironment global_lighting_environment;

    std::vector<Drawable*> drawables;
    std::vector<GUICanvas*> gui_canvases;
    std::vector<PointLightNode*> point_lights;

    void unblock_mouse_press();
    void draw_non_overlay_objects();

    void initialize_shadow_map();
    void update_shadow_map();
};

[[nodiscard]] inline RenderingServer& rs() {
    return RenderingServer::current();
}

[[nodiscard]] inline RenderingServer* rs_opt() {
    return RenderingServer::current_optional();
}
}