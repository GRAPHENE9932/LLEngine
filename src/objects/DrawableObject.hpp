#pragma once

#include <glm/mat4x4.hpp>
#include <GL/glew.h>

class DrawableObject {
public:
    virtual void draw(const glm::mat4& vp) = 0;
};
