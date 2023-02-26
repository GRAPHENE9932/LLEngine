#include "DrawableNode.hpp" // DrawableNode
#include "RenderingServer.hpp"

DrawableNode::DrawableNode(RenderingServer& rs, const Transform& p) :
    CompleteSpatialNode(p), rs(rs) {
    rs.register_drawable_node(this);
}

DrawableNode::~DrawableNode() {
    rs.unregister_drawable_node(this);
}
