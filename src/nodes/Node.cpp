#include "nodes/Node.hpp"
#include "nodes/RootNode.hpp"
#include "node_registration.hpp"
#include "logger.hpp"
#include "rendering/RenderingServer.hpp"

#include <fmt/format.h>

using namespace llengine;

void Node::enable_on_keyboard_key(bool enable) {
    if (!on_key_press_release_listening && enable) {
        get_rendering_server().get_window().subscribe_to_keyboard_key_event(*this);
    }
    else if (on_key_press_release_listening && !enable) {
        get_rendering_server().get_window().unsubscribe_from_keyboard_key_event(*this);
    }
    on_key_press_release_listening = enable;
}

void Node::enable() {
    enabled = true;
    if (!was_enabled_before) {
        _on_enable();
    }

    was_enabled_before = is_enabled();
}

void Node::disable() {
    enabled = false;
    if (was_enabled_before) {
        _on_disable();
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
    if (auto result = get_rendering_server_optional()) {
        return *result;
    }
    else {
        throw std::runtime_error(fmt::format(
            "Failed to get rendering server for a node \"{}\".",
            get_name()
        ));
    }
}

[[nodiscard]] RenderingServer* Node::get_rendering_server_optional() const {
    if (cached_rendering_server == nullptr) {
        if (RootNode* root = get_root_node_optional()) {
            cached_rendering_server = &root->get_rendering_server();
        }
    }

    return cached_rendering_server;
}

[[nodiscard]] RootNode& Node::get_root_node() const {
    if (RootNode* result = get_root_node_optional()) {
        return *result;
    }
    else {
        throw std::runtime_error(fmt::format(
            "Failed to get root node. Is node \"{}\" attached to the tree?",
            get_name()
        ));
    }
}

[[nodiscard]] BulletPhysicsServer& Node::get_bullet_physics_server() const {
    if (auto result = get_bullet_physics_server_optional()) {
        return *result;
    }
    else {
        throw std::runtime_error(fmt::format(
            "Failed to get physics server for a node \"{}\".",
            get_name()
        ));
    }
}

[[nodiscard]] BulletPhysicsServer* Node::get_bullet_physics_server_optional() const {
    if (cached_bullet_physics_server == nullptr) {
        if (RootNode* root = get_root_node_optional()) {
            cached_bullet_physics_server = &root->get_bullet_physics_server();
        }
    }

    return cached_bullet_physics_server;
}

void Node::_on_attachment_to_tree() {
    on_attachment_to_tree_without_start();

    try {
        start();
    }
    catch (const std::exception& e) {
        logger::error(fmt::format("start: {}", e.what()));
    }
    catch (...) {
        logger::error("Unknown error in start.");
    }
}

void Node::on_attachment_to_tree_without_start() {
    cached_rendering_server = nullptr;
    cached_bullet_physics_server = nullptr;
}

void Node::_on_parent_enable_disable(bool enabled) {
    parent_enabled = enabled;

    if (was_enabled_before && !enabled) {
        _on_disable();
    }
    else if (!was_enabled_before && enabled) {
        _on_enable();
    }

    was_enabled_before = is_enabled();
}

void Node::key_event_callback(Key key, Window::KeyEventType event_type) {
    if (event_type == Window::PRESS_EVENT) {
        on_keyboard_key_press(key);
    }
    else if (event_type == Window::RELEASE_EVENT) {
        on_keyboard_key_release(key);
    }
    else {
        throw std::runtime_error("Unknown keyboard event type.");
    }
}