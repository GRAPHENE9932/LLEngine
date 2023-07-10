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

protected:
    void on_attachment_to_tree() override final;
};
}