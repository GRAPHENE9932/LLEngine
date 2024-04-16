#include "nodes/rendering/DrawableCompleteSpatialNode.hpp"
#include "nodes/SpatialNode.hpp"
#include "rendering/RenderingServer.hpp"

using namespace llengine;

DrawableCompleteSpatialNode::~DrawableCompleteSpatialNode() {
    if (auto rendering_server = get_rendering_server_optional()) {
        rendering_server->unregister_drawable_node(this);
    }
}

void DrawableCompleteSpatialNode::_on_attachment_to_tree_without_start() {
    SpatialNode::_on_attachment_to_tree_without_start();
    get_rendering_server().register_drawable(this);
}