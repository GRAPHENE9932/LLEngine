#include "nodes/SpatialNode.hpp"
#include "node_registration.hpp"
#include "node_cast.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <utility>

using namespace llengine;

SpatialNode::~SpatialNode() {}

void SpatialNode::update_children() {
    for (const auto& child : children) {
        child->internal_update();
    }
}

void SpatialNode::add_child(std::unique_ptr<Node>&& child) {
    add_child(throwing_node_cast<SpatialNode>(std::move(child)));
}

void SpatialNode::add_child(std::unique_ptr<SpatialNode>&& child) {
    children_queued_to_add.push_back(std::move(child));
}

void SpatialNode::remove_child(const size_t index) {
    if (index >= children.size()) {
        throw std::out_of_range("Can not remove child: invalid child index specified.");
    }

    children_queued_to_remove.push_back(children[index].get());
}

void SpatialNode::remove_child(SpatialNode* const ptr) {
    const auto iter = std::find_if(
        children.begin(), children.end(),
        [&ptr](const auto& cur_unique) {
            return cur_unique.get() == ptr;
        }
    );
    if (iter == children.end()) {
        throw std::invalid_argument("Can't remove a non-existent child.");
    }

    children_queued_to_remove.push_back(ptr);
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
        spatial.add_child(child->copy());
    }
    for (const auto& child : children_queued_to_add) {
        spatial.add_child(child->copy());
    }
}

void SpatialNode::internal_update() {
    if (is_enabled()) {
        add_children_from_queue();
        remove_children_from_queue();
        update_children();
        update();
    }
}

void SpatialNode::on_attachment_to_tree() {
    Node::on_attachment_to_tree();
    
    std::for_each(
        children.begin(), children.end(),
        [] (const auto& child) {
            child->on_attachment_to_tree();
        }
    );
}

void SpatialNode::register_properties() {
    register_custom_property<SpatialNode>("spatial_node", "translation", &SpatialNode::set_translation_property);
    register_custom_property<SpatialNode>("spatial_node", "scale", &SpatialNode::set_scale_property);
    register_custom_property<SpatialNode>("spatial_node", "rotation", &SpatialNode::set_rotation_property);
}

void SpatialNode::internal_on_enable() {
    for (const auto& child : children) {
        child->on_parent_enable_disable(true);
    }
}

void SpatialNode::internal_on_disable() {
    for (const auto& child : children) {
        child->on_parent_enable_disable(false);
    }
}

void SpatialNode::add_children_from_queue() {
    if (children_queued_to_add.empty()) {
        return;
    }

    const bool attached_to_tree = is_attached_to_tree();

    for (auto& child_in_queue : children_queued_to_add) {
        child_in_queue->parent = this;
        children.emplace_back(std::move(child_in_queue));
        if (attached_to_tree) {
            children.back()->on_attachment_to_tree();
        }
    }

    children_queued_to_add.clear();
}

void SpatialNode::remove_children_from_queue() {
    if (children_queued_to_remove.empty()) {
        return;
    }

    std::size_t handled_children = 0;
    while (handled_children < children_queued_to_remove.size()) {
        std::size_t index_in_queue = 0;
        for (std::size_t index_in_children = 0; index_in_children < children.size();) {
            if (children[index_in_children].get() == children_queued_to_remove[index_in_queue]) {
                children.erase(children.begin() + index_in_children);
                index_in_queue++;
                handled_children++;
            }
            else {
                index_in_children++;
            }
        }
    }

    children_queued_to_remove.clear();
}