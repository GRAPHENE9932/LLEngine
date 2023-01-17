#include "RenderingServer.hpp" // RenderingServer
#include "DrawableNode.hpp" // DrawableNode

DrawableNode::DrawableNode(const SpatialParams& p, RenderingServer& rs) :
    CompleteSpatialNode(p), rendering_server(rs) {
    rendering_server.register_drawable_node(this);
}

DrawableNode::~DrawableNode() {
    rendering_server.unregister_drawable_node(this);
}
