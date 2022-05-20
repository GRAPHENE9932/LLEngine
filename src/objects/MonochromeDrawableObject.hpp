#pragma once

#include <glm/vec3.hpp>

#include "DrawableObject.hpp"

class MonochromeDrawableObject : public DrawableObject {
public:
    glm::vec3 color;
    GLuint normals_buf_id;
    std::vector<glm::vec3> normals_buf;

    MonochromeDrawableObject();
    MonochromeDrawableObject(glm::vec3 color);

    void init_normals_buf();
};
