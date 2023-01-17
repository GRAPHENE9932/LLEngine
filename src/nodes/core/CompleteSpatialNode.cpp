#include <algorithm> // std::find_if

#include <glm/gtx/transform.hpp> // glm::translate, glm::scale

#include "CompleteSpatialNode.hpp"
#include "common/core/GLTF.hpp" // GLTF

CompleteSpatialNode::CompleteSpatialNode(const SpatialParams& p) :
    spatial_params(p) {}

CompleteSpatialNode::~CompleteSpatialNode() {}

void CompleteSpatialNode::set_translation(const glm::vec3& new_trans) {
    spatial_params.translation = new_trans;
    needs_recalculation = true;
}

void CompleteSpatialNode::translate(const glm::vec3& translation) {
    set_translation(get_translation() + translation);
}

void CompleteSpatialNode::set_scale(const glm::vec3& new_scale) {
    spatial_params.scale = new_scale;
    needs_recalculation = true;
}

void CompleteSpatialNode::set_rotation(const glm::quat& new_rotation) {
    spatial_params.rotation = new_rotation;
    needs_recalculation = true;
}

glm::vec3 CompleteSpatialNode::get_translation() const noexcept {
    return spatial_params.translation;
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
    return spatial_params.scale;
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
    return spatial_params.rotation;
}

glm::quat CompleteSpatialNode::get_global_rotation() const noexcept {
    const auto parent = get_parent();

    if (parent) {
        return get_rotation() * get_global_rotation();
    }
    else {
        return get_rotation();
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

void CompleteSpatialNode::recalculate_matrix() const noexcept {
    cached_local_matrix = glm::translate(spatial_params.translation) *
            glm::scale(spatial_params.scale) *
            glm::toMat4(spatial_params.rotation);

    needs_recalculation = false;
}
