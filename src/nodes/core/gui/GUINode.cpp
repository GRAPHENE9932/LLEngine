#include "GUINode.hpp"
#include "RenderingServer.hpp"

GUINode::GUINode(RenderingServer& rs) : rs(rs) {
    rs.register_gui_node(this);
}

GUINode::~GUINode() {
    rs.unregister_gui_node(this);
}

GUITransform GUINode::get_absolute_transform() const noexcept {
    if (parent) {
        return parent->get_absolute_transform() * get_transform();
    }
    else {
        return get_transform();
    }
}

void GUINode::draw_children() {
    for (const auto& child_ptr : children) {
        child_ptr->draw();
    }
}

void GUINode::add_child(std::unique_ptr<GUINode>&& child) {
    child->parent = this;
    children.push_back(std::move(child));
}