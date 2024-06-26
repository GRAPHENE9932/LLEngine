#include "BulletNode.hpp"
#include "FadingPointLightNode.hpp"
#include "node_cast.hpp"
#include "nodes/rendering/ExplosionParticlesNode.hpp"

#include <fmt/format.h>
#include <logger.hpp>

static std::unique_ptr<llengine::ExplosionParticlesNode> particles = nullptr;

void BulletNode::start() {
    this->set_collision_checking_enabled(true);

    if (particles != nullptr) {
        return;
    }

    particles = std::make_unique<llengine::ExplosionParticlesNode>();
    particles->set_particles_count(64);
    particles->set_duration(0.5f);
    particles->set_particle_color({2000.0f, 2000.0f, 1.0f});
    particles->set_mesh(llengine::Mesh::get_cube());
    particles->set_scale_decline_curvature(1.0f);
    particles->set_scale({0.05f, 0.05f, 0.05f});
    particles->set_spreading_distance_deviation(0.35f);
    particles->set_spreading_distance(1.0f);

    auto particle_light = std::make_unique<FadingPointLightNode>();
    particle_light->set_transform(llengine::Transform());
    particle_light->set_original_color({100.0f, 100.0f, 0.0f});
    particle_light->set_duration(0.5f);

    particles->queue_add_child(std::move(llengine::node_cast<llengine::SpatialNode>(std::move(particle_light))));
}

void BulletNode::on_contact(llengine::BulletRigidBodyNode& collided_body) {
    if (!deleted && get_parent()) {
        std::unique_ptr<llengine::SpatialNode> copy = llengine::node_cast<llengine::SpatialNode>(particles->copy());
        copy->set_translation(get_translation());
        get_parent()->queue_add_child(std::move(copy));

        deleted = true;
        get_parent()->queue_remove_child(this);
    }
}

std::unique_ptr<llengine::Node> BulletNode::copy() const {
    std::unique_ptr<BulletNode> result {std::make_unique<BulletNode>()};
    copy_to(*result);
    return result;
}
