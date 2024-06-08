#include "nodes/rendering/PointLightNode.hpp" // PointLightNode
#include "rendering/RenderingServer.hpp" // RenderingServer

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <GL/glew.h>

using namespace llengine;

PointLightNode::~PointLightNode() {
    if (auto rendering_server = rs_opt()) {
        rendering_server->unregister_point_light(this);
    }
}

void PointLightNode::_on_attachment_to_tree_without_start() {
    SpatialNode::_on_attachment_to_tree_without_start();
    rs().register_point_light(this);
}

void PointLightNode::copy_to(Node& node) const {
    CompleteSpatialNode::copy_to(node);

    PointLightNode& pl_node = dynamic_cast<PointLightNode&>(node);
    pl_node.color = color;
}

std::unique_ptr<Node> PointLightNode::copy() const {
    std::unique_ptr<PointLightNode> result {std::make_unique<PointLightNode>()};
    copy_to(*result);
    return result;
}
