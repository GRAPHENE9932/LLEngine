#include <algorithm> // std::find_if

#include "nodes/CompleteSpatialNode.hpp"

using namespace llengine;

CompleteSpatialNode::CompleteSpatialNode(const Transform& p) :
    transform(p) {}

CompleteSpatialNode::~CompleteSpatialNode() {}

void CompleteSpatialNode::set_translation(const glm::vec3& new_trans) {
    transform.translation = new_trans;
    needs_recalculation = true;
}

void CompleteSpatialNode::translate(const glm::vec3& translation) {
    set_translation(get_translation() + translation);
}

void CompleteSpatialNode::set_scale(const glm::vec3& new_scale) {
    transform.scale = new_scale;
    needs_recalculation = true;
}

void CompleteSpatialNode::set_rotation(const glm::quat& new_rotation) {
    transform.rotation = new_rotation;
    needs_recalculation = true;
}

void CompleteSpatialNode::set_transform(const Transform& new_transform) {
    transform = new_transform;
}

glm::vec3 CompleteSpatialNode::get_translation() const noexcept {
    return transform.translation;
}

glm::vec3 CompleteSpatialNode::get_global_position() const noexcept {
    const auto parent = get_parent();

    if (parent) {
        return parent->get_global_position() + get_translation();
    }
    else {
        return get_translation();
    }
}

glm::vec3 CompleteSpatialNode::get_scale() const noexcept {
    return transform.scale;
}

glm::vec3 CompleteSpatialNode::get_global_scale() const noexcept {
    const auto parent = get_parent();

    if (parent) {
        return parent->get_global_scale() * get_scale();
    }
    else {
        return get_scale();
    }
}

glm::quat CompleteSpatialNode::get_rotation() const noexcept {
    return transform.rotation;
}

glm::quat CompleteSpatialNode::get_global_rotation() const noexcept {
    const auto parent = get_parent();

    if (parent) {
        return get_rotation() * parent->get_global_rotation();
    }
    else {
        return get_rotation();
    }
}

Transform CompleteSpatialNode::get_transform() const noexcept {
    return transform;
}

Transform CompleteSpatialNode::get_global_transform() const noexcept {
    const auto parent = get_parent();

    if (parent) {
        return get_transform() * parent->get_global_transform();
    }
    else {
        return get_transform();
    }
}

glm::mat4 CompleteSpatialNode::get_local_matrix() const noexcept {
    if (needs_recalculation)
        recalculate_matrix();

    return cached_local_matrix;
}

glm::mat4 CompleteSpatialNode::get_global_matrix() const noexcept {
    const auto parent = get_parent();

    if (parent == nullptr)
        return get_local_matrix();
    else
        return get_local_matrix() * parent->get_global_matrix();
}

std::unique_ptr<Node> CompleteSpatialNode::copy() const {
    std::unique_ptr<Node> result { std::make_unique<CompleteSpatialNode>() };
    copy_to(*result);
    return result;
}

void CompleteSpatialNode::recalculate_matrix() const noexcept {
    cached_local_matrix = transform.calculate_matrix();
    needs_recalculation = false;
}
