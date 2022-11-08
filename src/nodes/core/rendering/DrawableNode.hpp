#pragma once

#include "nodes/core/SpatialNode.hpp" // SpatialNode

#include <GL/glew.h> // GLuint

class RenderingServer;

class DrawableNode : public SpatialNode {
public:
    RenderingServer& rendering_server;

    DrawableNode(const SpatialParams& p, RenderingServer& rs);
    ~DrawableNode();

    virtual void draw() = 0;
    virtual GLuint get_program_id() const = 0;
};
