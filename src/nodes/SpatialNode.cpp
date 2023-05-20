#include "nodes/SpatialNode.hpp"

#include <algorithm>

using namespace llengine;

SpatialNode::~SpatialNode() {}

void SpatialNode::set_name(std::string_view new_name) {
    name = new_name;
}

[[nodiscard]] const std::string& SpatialNode::get_name() const {
    return name;
}

void SpatialNode::update_children() {
    for (const auto& child : children) {
        child->update();
    }
}

void SpatialNode::add_child(std::unique_ptr<SpatialNode>&& child) {
    child->parent = this;
    children.push_back(std::move(child));
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

const std::vector<std::unique_ptr<SpatialNode>>& SpatialNode::get_children() const {
    return children;
}

void SpatialNode::update() {
    update_children();
}
