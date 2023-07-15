#include "nodes/gui/GUICanvas.hpp"
#include "node_registration.hpp"
#include "nodes/SpatialNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "node_cast.hpp"

using namespace llengine;

void GUICanvas::on_attachment_to_tree() {
    SpatialNode::on_attachment_to_tree();

    std::for_each(
        gui_nodes.begin(), gui_nodes.end(),
        [] (const auto& child) {
            child->on_attachment_to_tree();
        }
    );
}

void GUICanvas::add_child(std::unique_ptr<Node>&& child) {
    if (auto gui_child = node_cast<GUINode>(std::move(child))) {
        add_gui_node(std::move(gui_child));
    }
    else {
        SpatialNode::add_child(throwing_node_cast<SpatialNode>(std::move(child)));
    }
}

void GUICanvas::add_gui_node(std::unique_ptr<GUINode>&& gui_node) {
    gui_node->assign_canvas_parent(*this);
    gui_nodes.emplace_back(std::move(gui_node));
}

void GUICanvas::remove_gui_node(std::size_t index) {
    if (index > gui_nodes.size()) {
        throw std::out_of_range("Can not remove child: invalid child index specified.");
    }

    gui_nodes.erase(gui_nodes.begin() + index);
}

void GUICanvas::remove_gui_node(GUINode* gui_node) {
    const auto iter = std::find_if(
        gui_nodes.begin(), gui_nodes.end(),
        [&gui_node](const auto& cur_unique) {
            return cur_unique.get() == gui_node;
        }
    );
    if (iter == gui_nodes.end()) {
        throw std::invalid_argument("Can't remove the non-existent child.");
    }

    gui_nodes.erase(iter);
}

[[nodiscard]] glm::vec2 GUICanvas::get_size() const {
    if (canvas_size.has_value()) {
        return *canvas_size;
    }
    else {
        return get_rendering_server().get_window().get_window_size();
    }
}

[[nodiscard]] glm::mat4 GUICanvas::get_mvp_matrix() const {
    if (is_screen_overlayed()) {
        return glm::mat4(1.0f);
    }
    else {
        return get_rendering_server().get_view_proj_matrix() * get_global_matrix();
    }
}

void GUICanvas::set_screen_overlayed_property(const NodeProperty& property) {
    set_screen_overlayed(property.get<bool>());
}

void GUICanvas::internal_update() {
    update_children();
    for (auto& gui_node : gui_nodes) {
        gui_node->internal_update();
    }

    update();
}

void GUICanvas::register_properties() {
    register_custom_property<GUICanvas>("gui_canvas", "screen_overlayed", &GUICanvas::set_screen_overlayed_property);
}