#include "nodes/gui/GUICanvas.hpp"
#include "rendering/RenderingServer.hpp"

llengine::GUICanvas::GUICanvas(llengine::RenderingServer& rs, const Transform& transform)
    : rs(rs), CompleteSpatialNode(transform) {

}

[[nodiscard]] glm::vec2 llengine::GUICanvas::get_size() const {
    if (canvas_size.has_value()) {
        return *canvas_size;
    }
    else {
        return rs.get_window().get_window_size();
    }
}

void llengine::GUICanvas::update() {
    for (auto& gui_node : gui_nodes) {
        gui_node->update();
    }
}