#pragma once

#include "nodes/core/CompleteSpatialNode.hpp"

#include <GL/glew.h> // GLuint

class RenderingServer;

class DrawableNode : public CompleteSpatialNode {
public:
    DrawableNode(RenderingServer& rs, const Transform& p = Transform());
    ~DrawableNode();

    virtual void draw() = 0;
    virtual GLuint get_program_id() const = 0;

protected:
    RenderingServer& rs;
};
