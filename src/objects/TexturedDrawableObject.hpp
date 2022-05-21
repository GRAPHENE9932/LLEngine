#pragma once

#include "DrawableObject.hpp"

class TexturedDrawableObject : public DrawableObject {
public:
    GLuint texture_id;

    GLuint normals_buf_id;
    std::vector<glm::vec3> normals_buf;
    GLuint uvs_buf_id;
    std::vector<glm::vec2> uvs_buf;

    TexturedDrawableObject();
    TexturedDrawableObject(GLuint texture_id);
    ~TexturedDrawableObject();

    void init_normals_buf();
    void init_uvs_buf();
};
