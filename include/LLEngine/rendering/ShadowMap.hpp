#pragma once

#include "nodes/rendering/CameraNode.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

#include <glm/mat4x4.hpp>

#include <optional>

namespace llengine {
class ShadowMap {
public:
    ShadowMap(glm::u32vec2 size);

    [[nodiscard]] TextureID get_texture_id() const {
        return texture_id;
    }
    /**
     * @brief Get user-defined unadjusted shadow map bias
     *
     * Returns the shadow map bias that coincides with real shadow
     * map bias used in the shader at 45 degrees between the normal
     * and a light source with shadow map size of 1024x1024.
     */
    [[nodiscard]] float get_bias() const {
        return bias_at_45_deg;
    }
    [[nodiscard]] float get_adjusted_bias_at_45_deg() const;

    [[nodiscard]] glm::u32vec2 get_size() const {
        return size;
    }

    void set_drawing_distance(float new_distance);
    [[nodiscard]] float get_drawing_distance() const {
        return drawing_distance;
    }

    void set_light_direction(const glm::vec3& light_direction) {
        this->light_direction = light_direction;
    }
    [[nodiscard]] const glm::vec3& get_light_direction() const {
        return light_direction;
    }

    void link_to_camera(const CameraNode& camera_node);

    [[nodiscard]] glm::mat4 get_view_proj_matrix() const;

private:
    const CameraNode* camera = nullptr;

    ManagedFramebufferID framebuffer;
    ManagedTextureID texture_id;
    float bias_at_45_deg = 0.0012f;
    glm::u32vec2 size { 1024, 1024 };
    float drawing_distance = 20.0f;
    glm::vec3 light_direction {-0.577350f, -0.577350f, -0.577350f};

    mutable std::optional<glm::mat4> view_proj_matrix_cache = std::nullopt;

    [[nodiscard]] glm::mat4 compute_view_proj_matrix() const;
    void prepare_for_drawing();
    void finish_drawing(bool enable_face_culling, FramebufferID default_framebuffer, glm::u32vec2 default_framebuffer_size);

    friend class RenderingServer;
};
}