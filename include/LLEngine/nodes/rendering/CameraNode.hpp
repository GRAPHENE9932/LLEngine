#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "nodes/CompleteSpatialNode.hpp"
#include "math/Frustum.hpp"

namespace llengine {
class RenderingServer;

/**
 * @brief Basic static camera. Everything it needs - spatial parameters,
 * field of view and display ratio.
 */
class CameraNode : public CompleteSpatialNode {
public:
    CameraNode();

    [[nodiscard]] const glm::mat4& get_view_matrix() const;
    [[nodiscard]] const glm::mat4& get_proj_matrix() const;
    [[nodiscard]] const glm::mat4& get_view_proj_matrix() const;

    void set_translation(const glm::vec3& new_trans) override;
    void translate(const glm::vec3& translation) override;
    void set_rotation(const glm::quat& new_rotation) override;

    /**
     * @brief Sets the field of view for the Y axis in radians.
     *
     * Field of view for the X axis depends on FOV for the Y axis and aspect ratio.
     */
    void set_field_of_view(float new_field_of_view);
    /**
     * @brief Sets the aspect ratio (X/Y).
     *
     * By default, aspect ratio is being calculated from the current window
     * framebuffer size. To restore automatic calculation use reset_aspect_ratio.
     * 
     * @see reset_aspect_ratio 
     */
    void set_aspect_ratio(float new_aspect_ratio);
    /**
     * @brief Removes the manually set aspect ratio, so camera can calculate it itself.
     */
    void reset_aspect_ratio();

    [[nodiscard]] glm::vec3 get_direction() const;

    void set_far_distance(float far_distance);
    [[nodiscard]] float get_far_distance() const;
    void set_near_distance(float near_distance);
    [[nodiscard]] float get_near_distance() const;

    [[nodiscard]] Frustum get_frustum() const;

protected:
    void on_attachment_to_tree_without_start() override final;

private:
    float field_of_view = glm::radians(90.0f);
    std::optional<float> manual_aspect_ratio = std::nullopt;
    float far_distance = 100.0f;
    float near_distance = 0.1f;

    mutable bool is_cached_view_matrix_valid = false;
    mutable bool is_cached_proj_matrix_valid = false;
    mutable bool is_cached_view_proj_matrix_valid = false;
    mutable glm::mat4 cached_view_matrix;
    mutable glm::mat4 cached_proj_matrix;
    mutable glm::mat4 cached_view_proj_matrix;
    mutable float previous_aspect_ratio;

    void recompute_view_matrix() const noexcept;
    void recompute_proj_matrix() const noexcept;

    [[nodiscard]] float get_aspect_ratio() const;
};
}