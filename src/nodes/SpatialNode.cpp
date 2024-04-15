#include "nodes/SpatialNode.hpp"
#include "node_registration.hpp"
#include "node_cast.hpp"
#include "logger.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <utility>

using namespace llengine;

SpatialNode::~SpatialNode() {}

void SpatialNode::update_children() {
    for (const auto& child : children) {
        child->_update();
    }
}

void SpatialNode::queue_add_child(std::unique_ptr<Node>&& child) {
    queue_add_child(throwing_node_cast<SpatialNode>(std::move(child)));
}

void SpatialNode::queue_add_child(std::unique_ptr<SpatialNode>&& child) {
    children_queued_to_add.push_back(std::move(child));
}

void SpatialNode::queue_remove_child(const size_t index) {
    if (index >= children.size()) {
        throw std::out_of_range("Can not remove child: invalid child index specified.");
    }

    children_queued_to_remove.push_back(children[index].get());
}

void SpatialNode::queue_remove_child(SpatialNode* const ptr) {
    const auto iter = std::find_if(
        children.begin(), children.end(),
        [&ptr](const auto& cur_unique) {
            return cur_unique.get() == ptr;
        }
    );
    if (iter == children.end()) {
        throw std::invalid_argument("Can't remove a non-existent child.");
    }

    if (
        std::find(
            children_queued_to_remove.begin(), children_queued_to_remove.end(), ptr
        ) == children_queued_to_remove.end()
    ) {
        children_queued_to_remove.push_back(ptr);
    }
}

SpatialNode* SpatialNode::get_parent() const {
    return parent;
}

[[nodiscard]] RootNode* SpatialNode::get_root_node_optional() const {
    if (parent) {
        return parent->get_root_node_optional();
    }
    else {
        return nullptr;
    }
}

const std::vector<std::unique_ptr<SpatialNode>>& SpatialNode::get_children() const {
    return children;
}

void SpatialNode::set_translation_property(const NodeProperty& property) {
    set_translation(property.get<glm::vec3>());
}

void SpatialNode::set_scale_property(const NodeProperty& property) {
    set_scale(property.get<glm::vec3>());
}

void SpatialNode::set_rotation_property(const NodeProperty& property) {
    set_rotation(property.get<glm::quat>());
}

void SpatialNode::copy_to(Node& node) const {
    Node::copy_to(node);

    SpatialNode& spatial = dynamic_cast<SpatialNode&>(node);
    spatial.set_transform(get_transform());
    for (const auto& child : children) {
        spatial.queue_add_child(child->copy());
    }
    for (const auto& child : children_queued_to_add) {
        spatial.queue_add_child(child->copy());
    }
}

void SpatialNode::_update() {
    if (is_enabled()) {
        add_children_from_queue();
        remove_children_from_queue();

        try {
            update_children();
            update();
        }
        catch (const std::exception& e) {
            logger::error(fmt::format("update: {}", e.what()));
        }
        catch (...) {
            logger::error("Unknown error in update.");
        }
    }
}

void SpatialNode::on_attachment_to_tree_without_start() {
    Node::on_attachment_to_tree_without_start();

    std::for_each(
        children.begin(), children.end(),
        [] (const auto& child) {
            child->_on_attachment_to_tree();
        }
    );
}

void SpatialNode::register_properties() {
    register_custom_property<SpatialNode>("spatial_node", "translation", &SpatialNode::set_translation_property);
    register_custom_property<SpatialNode>("spatial_node", "scale", &SpatialNode::set_scale_property);
    register_custom_property<SpatialNode>("spatial_node", "rotation", &SpatialNode::set_rotation_property);
}

void SpatialNode::_on_enable() {
    for (const auto& child : children) {
        child->_on_parent_enable_disable(true);
    }
}

void SpatialNode::_on_disable() {
    for (const auto& child : children) {
        child->_on_parent_enable_disable(false);
    }
}

void SpatialNode::add_children_from_queue() {
    if (children_queued_to_add.empty()) {
        return;
    }

    const bool attached_to_tree = is_attached_to_tree();

    for (std::size_t i = 0; i < children_queued_to_add.size(); i++) {
        children_queued_to_add[i]->parent = this;
        children.emplace_back(std::move(children_queued_to_add[i]));
        if (attached_to_tree) {
            children.back()->_on_attachment_to_tree();
        }
    }

    children_queued_to_add.clear();
}

void SpatialNode::remove_children_from_queue() {
    for (auto child_ptr : children_queued_to_remove) {
        const auto iter = std::find_if(
            children.begin(), children.end(), [&child_ptr] (const auto& unique_ptr) {
                return unique_ptr.get() == child_ptr;
            }
        );

        if (iter == children.end()) {
            continue;
        }
        children.erase(iter);
    }

    children_queued_to_remove.clear();
}