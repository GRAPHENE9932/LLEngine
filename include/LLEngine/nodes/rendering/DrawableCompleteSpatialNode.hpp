#pragma once

#include "nodes/CompleteSpatialNode.hpp"
#include "nodes/rendering/Drawable.hpp"

namespace llengine {
class RenderingServer;

class DrawableCompleteSpatialNode : public Drawable, public CompleteSpatialNode {
public:
    ~DrawableCompleteSpatialNode();

protected:
    void on_attachment_to_tree() override final;
};
}