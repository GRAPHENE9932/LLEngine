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
    child->parent = this;
    children.push_back(std::move(child));
    if (is_attached_to_tree()) {
        children.back()->on_attachment_to_tree();
    }
}

void SpatialNode::remove_child(const size_t index) {
    if (index > children.size()) {
        throw std::out_of_range("Can not remove child: invalid child index specified.");
    }

    auto iter = children.begin() + index;
    iter->get()->parent = nullptr;
    children.erase(iter);
}

void SpatialNode::remove_child(SpatialNode* const ptr) {
    const auto iter = std::find_if(
        children.begin(), children.end(),
        [&ptr](const auto& cur_unique) {
            return cur_unique.get() == ptr;
        }
    );
    if (iter == children.end())
        throw std::invalid_argument("Can't remove the non-existent child.");

    iter->get()->parent = nullptr;
    children.erase(iter);
}

SpatialNode* SpatialNode::get_parent() const {
    return parent;
}

[[nodiscard]] const RootNode& SpatialNode::get_root_node() const {
    if (parent) {
        return parent->get_root_node();
    }
    else {
        throw std::runtime_error(fmt::format(
            "Failed to get root node. Is node \"{}\" attached to the tree?",
            get_name()
        ));
    }
}

[[nodiscard]] RootNode& SpatialNode::get_root_node() {
    return const_cast<RootNode&>(std::as_const(*this).get_root_node());
}

[[nodiscard]] bool SpatialNode::is_attached_to_tree() const {
    if (parent) {
        return parent->is_attached_to_tree();
    }
    else {
        return false;
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

void SpatialNode::internal_update() {
    update_children();
    update();
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