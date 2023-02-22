#include "DrawableNode.hpp" // DrawableNode
#include "RenderingServer.hpp"

DrawableNode::DrawableNode(const Transform& p) :
    CompleteSpatialNode(p) {
    RenderingServer::get_instance().register_drawable_node(this);
}

DrawableNode::~DrawableNode() {
    RenderingServer::get_instance().unregister_drawable_node(this);
}
