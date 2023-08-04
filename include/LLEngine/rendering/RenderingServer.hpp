#pragma once

#include <set> // std::set
#include <memory> // std::shared_ptr
#include <chrono> // std::chrono::high_resolution_clock
#include <cstdint> // uint64_t
#include <optional> // std::optional
#include <functional>

#include <glm/vec2.hpp> // glm::ivec2
#include <glm/mat4x4.hpp> // glm::mat4

#include "rendering/Window.hpp" // Window
#include "rendering/Skybox.hpp" // Skybox
#include "rendering/shaders/ShaderHolder.hpp"

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

    [[nodiscard]] std::optional<std::reference_wrapper<const Texture>>
    get_irradiance_map(const glm::vec3& obj_position);

    [[nodiscard]] std::optional<std::reference_wrapper<const Texture>>
    get_prefiltered_specular_map(const glm::vec3& obj_position);

    [[nodiscard]] const Texture& get_brdf_integration_map();

    [[nodiscard]] bool have_environment_cubemap();

private:
    Window window;
    std::function<void(float)> update_callback;

    bool mouse_button_blocked = false;
    bool stop_requested = false;

    // Time point of the last frame.
    std::chrono::high_resolution_clock::time_point prev_frame_time;
    float delta_time = 1.0f;
    std::optional<glm::vec3> sun_direction;

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
};
}