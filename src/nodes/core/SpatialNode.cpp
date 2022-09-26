#include <algorithm> // std::find_if

#include <glm/gtx/transform.hpp> // glm::translate, glm::scale

#include "SpatialNode.hpp" // SpatialNode
#include "SceneTree.hpp" // SceneTree
#include "common/GLTF.hpp" // GLTF

SpatialNode::SpatialNode(const SpatialParams& p, SceneTree& scene_tree) :
        spatial_params(p), scene_tree(scene_tree) {

}

SpatialNode::~SpatialNode() {

}

void SpatialNode::update_children() {
    for (const auto& child : children)
        child->update();
}

void SpatialNode::add_child(std::unique_ptr<SpatialNode> child) {
    children.push_back(std::move(child));
    children.back()->register_myself(this);
}

void SpatialNode::remove_child(const size_t index) {
    SpatialNode* const ptr = children.at(index).get();
    children.erase(children.begin() + index);
    scene_tree.handle_node_removal(ptr);
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

    children.erase(iter);
    scene_tree.handle_node_removal(ptr);
}

void SpatialNode::register_myself(SpatialNode* parent) {
    scene_tree.register_node(this, parent);
}

const std::vector<std::unique_ptr<SpatialNode>>& SpatialNode::get_children() {
    return children;
}

void SpatialNode::update() {
    update_children();
}

void SpatialNode::set_translation(const glm::vec3& new_trans) {
    spatial_params.translation = new_trans;
    needs_recalculation = true;
}

void SpatialNode::translate(const glm::vec3 &translation) {
    set_translation(get_translation() + translation);
}

void SpatialNode::set_scale(const glm::vec3& new_scale) {
    spatial_params.scale = new_scale;
    needs_recalculation = true;
}

void SpatialNode::set_rotation(const glm::quat& new_rotation) {
    spatial_params.rotation = new_rotation;
    needs_recalculation = true;
}

const glm::vec3& SpatialNode::get_translation() const noexcept {
    return spatial_params.translation;
}

const glm::vec3 & SpatialNode::get_scale() const noexcept {
    return spatial_params.scale;
}

const glm::quat& SpatialNode::get_rotation() const noexcept {
    return spatial_params.rotation;
}

glm::mat4 SpatialNode::get_local_matrix() {
    if (needs_recalculation)
        recalculate_matrix();

    return local_matrix;
}

glm::mat4 SpatialNode::get_global_matrix() {
    if (parent == nullptr)
        return get_local_matrix();
    else
        return get_local_matrix() * parent->get_global_matrix();
}

void SpatialNode::recalculate_matrix() {
    local_matrix = glm::translate(spatial_params.translation) *
            glm::scale(spatial_params.scale) *
            glm::toMat4(spatial_params.rotation);

    needs_recalculation = false;
}
