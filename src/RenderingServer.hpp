#pragma once

#include <set> // std::set
#include <memory> // std::shared_ptr
#include <chrono> // std::chrono::high_resolution_clock
#include <cstdint> // uint64_t
#include <optional> // std::optional

#include <glm/vec2.hpp> // glm::ivec2
#include <glm/mat4x4.hpp> // glm::mat4

#include "common/core/IWindow.hpp" // IWindow
#include "common/core/Skybox.hpp" // Skybox
#include "common/core/shaders/ShaderManager.hpp" // ShaderManager

class Texture;
class RenderingNode;
class CameraNode;
class DrawableNode;
struct PointLightNode;
struct SpotLight;

/**
 * @brief Stores non-owning pointers to all
 * rendering-related nodes and renders them.
 */
class RenderingServer {
public:
    RenderingServer(glm::ivec2 window_extents);
    ~RenderingServer();

    void set_skybox(const std::shared_ptr<Texture>& texture);
    void set_root_node(SpatialNode* root_node);

    void main_loop();

    /**
     * @brief Must be called by every DrawableNode upon its creation.
     *
     * Does nothing if drawable_node is nullptr.
     * @param drawable_node new freshly-created drawable node.
     */
    void register_drawable_node(DrawableNode* drawable_node) noexcept;
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
    void unregister_drawable_node(DrawableNode* drawable_node) noexcept;
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
    glm::mat4 get_view_matrix() const noexcept;

    /**
     * @brief Returns projection matrix from the current camera.
     *
     * If camera is missing, returns identity matrix.
     */
    glm::mat4 get_proj_matrix() const noexcept;
    
    /**
     * @brief Get the projection matrix multiplied by view matrix.
     *
     * Both these matrices can be retrieved from
     * get_view_matrix() and get_proj_matrix().
     */
    glm::mat4 get_view_proj_matrix() const noexcept;

    const std::set<PointLightNode*>& get_point_lights() const noexcept {
        return point_lights;
    }

    const std::set<DrawableNode*> get_drawable_nodes() const {
        return drawable_nodes;
    }

    float get_delta_time() const {
        return delta_time;
    }

    IWindow& get_window() {
        return window;
    }

    ShaderManager& get_shader_manager() {
        return shader_manager;
    }

private:
    IWindow& window;
    ShaderManager shader_manager;

    // Time point of the last frame.
    std::chrono::high_resolution_clock::time_point prev_frame_time;
    float delta_time = 1.0f;

    // Non-owning pointer to the current camera node.
    CameraNode* camera = nullptr;
    std::unique_ptr<Skybox> skybox = nullptr;
    std::set<DrawableNode*> drawable_nodes;
    std::set<PointLightNode*> point_lights;

    // Non-owning pointer to the root node. Used to invoke update().
    SpatialNode* root_node = nullptr;
};
