#include "nodes/Node.hpp"
#include "nodes/RootNode.hpp"
#include "node_registration.hpp"

#include <fmt/format.h>

using namespace llengine;

void Node::register_properties() {
    register_custom_property<Node>("node", "name", &Node::set_name_property);
}

[[nodiscard]] RenderingServer& Node::get_rendering_server() const {
    if (cached_rendering_server == nullptr) {
        try {
            cached_rendering_server = &get_root_node().get_rendering_server();
        }
        catch (const std::exception& e) {
            throw std::runtime_error(fmt::format(
                "Failed to get rendering server for a node because: \"{}\".",
                e.what()
            ));
        }
    }

    return *cached_rendering_server;
}

[[nodiscard]] BulletPhysicsServer& Node::get_bullet_physics_server() const {
    if (cached_bullet_physics_server == nullptr) {
        try {
            cached_bullet_physics_server = &get_root_node().get_bullet_physics_server();
        }
        catch (const std::exception& e) {
            throw std::runtime_error(fmt::format(
                "Failed to get physics server for a node because: \"{}\".",
                e.what()
            ));
        }
    }

    return *cached_bullet_physics_server;
}

void Node::on_attachment_to_tree() {
    cached_rendering_server = nullptr;
    cached_bullet_physics_server = nullptr;
}