#pragma once

#include "nodes/core/CompleteSpatialNode.hpp"

#include <GL/glew.h> // GLuint

class DrawableNode : public CompleteSpatialNode {
public:
    DrawableNode(const Transform& p);
    ~DrawableNode();

    virtual void draw() = 0;
    virtual GLuint get_program_id() const = 0;
};
