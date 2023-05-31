#pragma once

#include "nodes/CompleteSpatialNode.hpp"
#include "nodes/rendering/Drawable.hpp"

namespace llengine {
class RenderingServer;

class DrawableCompleteSpatialNode : public Drawable, public CompleteSpatialNode {
public:
    DrawableCompleteSpatialNode(RenderingServer& rs, const Transform& transform = Transform());
    ~DrawableCompleteSpatialNode();

protected:
    RenderingServer& rs;
};
}