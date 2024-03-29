#include "nodes/gui/CompleteGUINode.hpp"

using namespace llengine;

[[nodiscard]] GUITransform CompleteGUINode::get_transform() const {
    return transform;
}

[[nodiscard]] glm::vec2 CompleteGUINode::get_absolute_size() const {
    switch (transform.size_mode) {
    case GUITransform::SizeMode::ABSOLUTE:
        return transform.size;
        break;
    case GUITransform::SizeMode::RELATIVE:
        return get_parent_size() * transform.size;
        break;
    default:
        throw std::runtime_error("Invalid size mode in a GUI node.");
    }
}

void CompleteGUINode::set_transform(const GUITransform& transform) {
    this->transform = transform;
}