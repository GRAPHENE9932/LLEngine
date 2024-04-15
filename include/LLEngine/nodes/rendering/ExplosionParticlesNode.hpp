#pragma once

#include "rendering/Mesh.hpp"
#include "NodeProperty.hpp"
#include "datatypes.hpp"
#include "DrawableCompleteSpatialNode.hpp"

#include <glm/vec3.hpp>

#include <cstdint>

namespace llengine {
class ExplosionParticlesNode : public DrawableCompleteSpatialNode {
public:
    ExplosionParticlesNode() = default;
    ~ExplosionParticlesNode() = default;

    void set_mesh(const std::shared_ptr<const Mesh>& mesh);
    [[nodiscard]] const std::shared_ptr<const Mesh>& get_mesh() const;

    void set_particle_color(const glm::vec3& particle_color);
    void set_particle_color_property(const NodeProperty& property);
    [[nodiscard]] const glm::vec3& get_particle_color() const;

    void set_duration(float duration);
    void set_duration_property(const NodeProperty& property);
    [[nodiscard]] float get_duration() const;

    void set_spreading_distance(float spreading_distance);
    void set_spreading_distance_property(const NodeProperty& property);
    [[nodiscard]] float get_spreading_distance() const;

    /**
     * At 0.0 the particle spreading distance will not deviate at all.
     * At 1.0 it will deviate in range from 0 to 2 * spreading_distance.
     */
    void set_spreading_distance_deviation(float spreading_distance_random_range);
    void set_spreading_distance_deviation_property(const NodeProperty& property);
    [[nodiscard]] float get_spreading_distance_deviation() const;

    /**
     * @brief at 1.0, scale will decrease linearly, at 0.0 scale will not decrease.
     *
     * Sets the c parameter of equation
     * scale = 1 - phase^(1/c).
     * That means, that at curvature of 1.0, scale will decrease linearly,
     * at curvature of 0.0 scale will not decrease at all.
     */
    void set_scale_decline_curvature(float scale_decline_curvature);
    /**
     * @see set_scale_decline_curvature.
     */
    void set_scale_decline_curvature_property(const NodeProperty& property);
    /**
     * @see set_scale_decline_curvature.
     */
    [[nodiscard]] float get_scale_decline_curvature() const;

    void set_particles_count(std::uint32_t particles_count);
    void set_particles_count_property(const NodeProperty& property);
    [[nodiscard]] std::uint32_t get_particles_count() const;

    void draw() override;

    void copy_to(Node& node) const override;
    [[nodiscard]] std::unique_ptr<Node> copy() const override;

    [[nodiscard]] ShaderID get_program_id() const override;

protected:
    void _on_attachment_to_tree_without_start() override;

private:
    std::shared_ptr<const Mesh> particle_mesh = nullptr;
    glm::vec3 particle_color = {0.0f, 0.0f, 0.0f};
    float duration = 1.0f;
    float spreading_distance = 1.0f;
    float spreading_distance_deviation = 0.5f;
    float scale_decline_curvature = 0.0f;
    std::uint32_t particles_count = 0;
    float phase = 0.0f;

    VertexArrayID vao_id = 0;
    BufferID target_positions_buffer = 0;

    void initialize_vao();
    void reinitialize_target_positions_buffer();
};
}