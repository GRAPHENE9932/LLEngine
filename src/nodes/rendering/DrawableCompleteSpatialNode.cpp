#include "nodes/rendering/DrawableCompleteSpatialNode.hpp"
#include "nodes/SpatialNode.hpp"
#include "rendering/RenderingServer.hpp"

using namespace llengine;

DrawableCompleteSpatialNode::~DrawableCompleteSpatialNode() {
    if (auto rendering_server = rs_opt()) {
        rendering_server->unregister_drawable_node(this);
    }
}

void DrawableCompleteSpatialNode::_on_attachment_to_tree_without_start() {
    SpatialNode::_on_attachment_to_tree_without_start();
    rs().register_drawable(this);
}