#include "nodes/Node.hpp"
#include "nodes/RootNode.hpp"
#include "node_registration.hpp"

#include <fmt/format.h>

using namespace llengine;

void Node::enable() {
    enabled = true;
    if (!was_enabled_before) {
        internal_on_enable();
    }

    was_enabled_before = is_enabled();
}

void Node::disable() {
    enabled = false;
    if (was_enabled_before) {
        internal_on_disable();
    }

    was_enabled_before = is_enabled();
}

void Node::set_enabled_property(const NodeProperty& property) {
    bool to_enable = property.get<bool>();
    if (to_enable) {
        enable();
    }
    else {
        disable();
    }
}

[[nodiscard]] bool Node::is_enabled() const {
    return parent_enabled && enabled;
}

void Node::copy_to(Node& node) const {
    node.enabled = enabled;
    node.set_name(name);
}

void Node::register_properties() {
    register_custom_property<Node>("node", "name", &Node::set_name_property);
    register_custom_property<Node>("node", "enabled", &Node::set_enabled_property);
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

void Node::on_parent_enable_disable(bool enabled) {
    parent_enabled = enabled;

    if (was_enabled_before && !enabled) {
        internal_on_disable();
    }
    else if (!was_enabled_before && enabled) {
        internal_on_enable();
    }

    was_enabled_before = is_enabled();
}