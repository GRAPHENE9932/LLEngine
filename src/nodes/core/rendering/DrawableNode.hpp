#pragma once

#include <GL/glew.h> // GLuint

#include "nodes/core/SpatialNode.hpp" // SpatialNode

class DrawableNode : public SpatialNode {
public:
    DrawableNode(const SpatialParams& params, SceneTree& scene_tree) :
                 SpatialNode(params, scene_tree) {};

    virtual void draw() = 0;
    virtual GLuint get_program_id() const = 0;
    virtual ~DrawableNode() = default;
};
