#include "nodes/rendering/ExplosionParticlesNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "rendering/Shader.hpp"
#include "random.hpp"

#include <GL/glew.h>

#include <random>
#include <numbers>

namespace llengine {
void ExplosionParticlesNode::set_mesh(const std::shared_ptr<const Mesh>& mesh) {
    particle_mesh = mesh;
    if (is_attached_to_tree()) {
        reinitialize_target_positions_buffer();
    }
}

[[nodiscard]] const std::shared_ptr<const Mesh>& ExplosionParticlesNode::get_mesh() const {
    return particle_mesh;
}

void ExplosionParticlesNode::set_particle_color(const glm::vec3& particle_color) {
    this->particle_color = particle_color;
}

void ExplosionParticlesNode::set_particle_color_property(const NodeProperty& property) {
    this->particle_color = property.get<glm::vec3>();
}

[[nodiscard]] const glm::vec3& ExplosionParticlesNode::get_particle_color() const {
    return particle_color;
}

void ExplosionParticlesNode::set_duration(float duration) {
    this->duration = duration;
}

void ExplosionParticlesNode::set_duration_property(const NodeProperty& property) {
    this->duration = property.get<float>();
}

[[nodiscard]] float ExplosionParticlesNode::get_duration() const {
    return duration;
}

void ExplosionParticlesNode::set_spreading_distance(float spreading_distance) {
    this->spreading_distance = spreading_distance;
    if (is_attached_to_tree()) {
        reinitialize_target_positions_buffer();
    }
}

void ExplosionParticlesNode::set_spreading_distance_property(const NodeProperty& property) {
    set_spreading_distance(property.get<float>());
}

[[nodiscard]] float ExplosionParticlesNode::get_spreading_distance() const {
    return spreading_distance;
}

void ExplosionParticlesNode::set_spreading_distance_deviation(float spreading_distance_random_range) {
    this->spreading_distance_deviation = spreading_distance_random_range;
    if (is_attached_to_tree()) {
        reinitialize_target_positions_buffer();
    }
}

void ExplosionParticlesNode::set_spreading_distance_deviation_property(const NodeProperty &property) {
    set_spreading_distance_deviation(property.get<float>());
}

[[nodiscard]] float ExplosionParticlesNode::get_spreading_distance_deviation() const {
    return spreading_distance_deviation;
}

void ExplosionParticlesNode::set_scale_decline_curvature(float scale_decline_curvature) {
    this->scale_decline_curvature = scale_decline_curvature;
}

void ExplosionParticlesNode::set_scale_decline_curvature_property(const NodeProperty& property) {
    this->scale_decline_curvature = property.get<float>();
}

[[nodiscard]] float ExplosionParticlesNode::get_scale_decline_curvature() const {
    return scale_decline_curvature;
}

void ExplosionParticlesNode::set_particles_count(std::uint32_t particles_count) {
    this->particles_count = particles_count;
    if (is_attached_to_tree()) {
        reinitialize_target_positions_buffer();
    }
}

void ExplosionParticlesNode::set_particles_count_property(const NodeProperty& property) {
    set_particles_count(property.get<std::uint32_t>());
}

[[nodiscard]] std::uint32_t ExplosionParticlesNode::get_particles_count() const {
    return particles_count;
}

using ExplosionParticlesShader = Shader<
    "center_mvp", "particle_scale_inverse", "spreading_distance",
    "scale_decline_power", "phase", "particles_count", "particle_color"
>;
static std::unique_ptr<ExplosionParticlesShader> shader = nullptr;

static void ensure_shader_is_initialized() {
    if (shader == nullptr) {
        shader = std::make_unique<ExplosionParticlesShader>(
            #include "shaders/explosion_particles.vert"
            ,
            #include "shaders/explosion_particles.frag"
        );
    }
}

void ExplosionParticlesNode::draw() {
    if (particle_mesh == nullptr) {
        return;
    }

    const glm::mat4 mvp = get_rendering_server().get_view_proj_matrix() * get_global_matrix();

    ensure_shader_is_initialized();
    shader->use_shader();
    shader->set_mat4<"center_mvp">(mvp);
    shader->set_vec3<"particle_scale_inverse">(1.0f / get_scale());
    shader->set_float<"spreading_distance">(get_spreading_distance());
    shader->set_float<"scale_decline_power">(1.0f / get_scale_decline_curvature());
    shader->set_float<"phase">(phase);
    shader->set_uint<"particles_count">(particles_count);
    shader->set_vec3<"particle_color">(particle_color);

    glBindVertexArray(vao_id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_mesh->get_indices_id());
    glDrawElementsInstanced(
        GL_TRIANGLES, particle_mesh->get_amount_of_vertices(), particle_mesh->get_indices_type(),
        nullptr, particles_count
    );
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    phase += get_rendering_server().get_delta_time() / duration;

    if (phase >= 1.0f) {
        get_parent()->queue_remove_child(this);
    }
}

void ExplosionParticlesNode::copy_to(Node& node) const {
    DrawableCompleteSpatialNode::copy_to(node);

    ExplosionParticlesNode& ep_node = dynamic_cast<ExplosionParticlesNode&>(node);
    ep_node.set_mesh(get_mesh());
    ep_node.set_particle_color(get_particle_color());
    ep_node.set_duration(get_duration());
    ep_node.set_spreading_distance(get_spreading_distance());
    ep_node.set_scale_decline_curvature(get_scale_decline_curvature());
    ep_node.set_particles_count(get_particles_count());
    ep_node.set_spreading_distance_deviation(get_spreading_distance_deviation());
}

[[nodiscard]] std::unique_ptr<Node> ExplosionParticlesNode::copy() const {
    std::unique_ptr<Node> result = std::make_unique<ExplosionParticlesNode>();
    copy_to(*result);
    return result;
}

[[nodiscard]] ShaderID ExplosionParticlesNode::get_program_id() const {
    if (shader == nullptr) {
        return 0;
    }

    return shader->get_program_id();
}

void ExplosionParticlesNode::on_attachment_to_tree_without_start() {
    DrawableCompleteSpatialNode::on_attachment_to_tree_without_start();

    initialize_vao();
    reinitialize_target_positions_buffer();
}

[[nodiscard]] static glm::vec3 generate_random_point_on_sphere() {
    float phi = random::float_gaussian_distribution(0.0f, 0.9f);
    if (random::generate_bool()) {
        phi = glm::radians(180.0f) - phi;
    }

    float theta = random::float_in_range(glm::radians(-180.0f), glm::radians(180.0f));

    // Convert to cartesian coordinates.
    glm::vec3 result = glm::vec3(
        std::cos(phi) * std::sin(theta),
        std::sin(phi) * std::sin(theta),
        std::cos(theta)
    );

    return result;
}

static void compute_target_positions(
    std::vector<glm::vec3>& result, float spreading_distance,
    float spreading_distance_deviation, std::uint32_t particles_count
) {
    assert(result.size() == particles_count);

    for (glm::vec3& point : result) {
        point = generate_random_point_on_sphere() *
            random::float_in_range(1.0f - spreading_distance_deviation, 1.0f + spreading_distance_deviation);
    }
}

void ExplosionParticlesNode::initialize_vao() {
    if (vao_id != 0) {
        glDeleteVertexArrays(1, &vao_id);
    }

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    if (particle_mesh != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, particle_mesh->get_vertices_id());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (target_positions_buffer != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, target_positions_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribDivisor(1, 1);
    }
}

void ExplosionParticlesNode::reinitialize_target_positions_buffer() {
    if (target_positions_buffer != 0) {
        glDeleteBuffers(1, &target_positions_buffer);
    }

    std::vector<glm::vec3> target_positions(particles_count);
    compute_target_positions(target_positions, spreading_distance, spreading_distance_deviation, particles_count);

    glGenBuffers(1, &target_positions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, target_positions_buffer);
    glBufferData(GL_ARRAY_BUFFER, target_positions.size() * sizeof(glm::vec3), target_positions.data(), GL_STATIC_DRAW);

    if (vao_id != 0) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribDivisor(1, 1);
    }
}
}