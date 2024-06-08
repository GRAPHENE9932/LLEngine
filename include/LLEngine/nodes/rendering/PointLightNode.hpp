#pragma once

#include <glm/vec3.hpp> // glm::vec3

#include "nodes/CompleteSpatialNode.hpp"

namespace llengine {
class RenderingServer;

struct PointLightNode : public CompleteSpatialNode {
    glm::vec3 color;

    PointLightNode() = default;
    PointLightNode(const PointLightNode& other) = delete;
    PointLightNode(PointLightNode&& other) = delete;
    PointLightNode& operator=(const PointLightNode& other) = delete;
    PointLightNode& operator=(PointLightNode&& other) = delete;
    ~PointLightNode();

    void _on_attachment_to_tree_without_start() final override;

    virtual void copy_to(Node& node) const override;
    virtual std::unique_ptr<Node> copy() const override;
};
}