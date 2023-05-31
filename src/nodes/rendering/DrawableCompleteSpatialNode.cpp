#include "nodes/rendering/DrawableCompleteSpatialNode.hpp"
#include "rendering/RenderingServer.hpp"

using namespace llengine;

DrawableCompleteSpatialNode::DrawableCompleteSpatialNode(RenderingServer& rs, const Transform& transform) :
    rs(rs), CompleteSpatialNode(transform) {
    rs.register_drawable(this);
}

DrawableCompleteSpatialNode::~DrawableCompleteSpatialNode() {
    rs.unregister_drawable_node(this);
}
