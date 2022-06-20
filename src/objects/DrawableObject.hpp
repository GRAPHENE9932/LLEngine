#pragma once

#include <GL/glew.h>

class DrawableObject {
public:
    virtual void draw(GLfloat* camera_mvp) = 0;
};
