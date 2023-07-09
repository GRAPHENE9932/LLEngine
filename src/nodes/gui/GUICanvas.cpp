#include "nodes/gui/GUICanvas.hpp"
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
    
}

bool GUICanvas::remove_gui_node(std::size_t index) {

}

bool GUICanvas::remove_gui_node(GUINode* const gui_node) {

}

[[nodiscard]] glm::vec2 GUICanvas::get_size() const {
    if (canvas_size.has_value()) {
        return *canvas_size;
    }
    else {
        return get_rendering_server().get_window().get_window_size();
    }
}

void GUICanvas::update() {
    for (auto& gui_node : gui_nodes) {
        gui_node->update();
    }
}
