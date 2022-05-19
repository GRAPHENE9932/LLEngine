#pragma once

#include <vector>

#include <GL/glew.h>

#include "SpatialObject.hpp"

class DrawableObject : public SpatialObject {
public:
    GLuint vertex_buf_id;
    std::vector<GLfloat> vertex_buf;

    void init_vertex_buf();
};
