#pragma once

#include "nodes/CompleteSpatialNode.hpp"
#include "nodes/rendering/Drawable.hpp"

namespace llengine {
class RenderingServer;

class DrawableCompleteSpatialNode : public Drawable, public CompleteSpatialNode {
public:
    DrawableCompleteSpatialNode() = default;
    DrawableCompleteSpatialNode(const DrawableCompleteSpatialNode& other) = delete;
    DrawableCompleteSpatialNode(DrawableCompleteSpatialNode&& other) = delete;
    DrawableCompleteSpatialNode& operator=(const DrawableCompleteSpatialNode& other) = delete;
    DrawableCompleteSpatialNode& operator=(DrawableCompleteSpatialNode&& other) = delete;
    ~DrawableCompleteSpatialNode();

    [[nodiscard]] bool is_enabled() const final override {
        return CompleteSpatialNode::is_enabled();
    }

protected:
    void on_attachment_to_tree_without_start() override final;
};
}