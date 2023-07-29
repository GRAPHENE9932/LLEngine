#include "nodes/rendering/DrawableCompleteSpatialNode.hpp"
#include "nodes/SpatialNode.hpp"
#include "rendering/RenderingServer.hpp"

using namespace llengine;

DrawableCompleteSpatialNode::~DrawableCompleteSpatialNode() {
    if (auto rendering_server = get_rendering_server_optional()) {
        rendering_server->unregister_drawable_node(this);
    }
}

void DrawableCompleteSpatialNode::on_attachment_to_tree() {
    SpatialNode::on_attachment_to_tree();
    get_rendering_server().register_drawable(this);

    const auto& children = get_children();
    std::for_each(
        children.begin(), children.end(),
        [] (const auto& child) {
            child->on_attachment_to_tree();
        }
    );
}