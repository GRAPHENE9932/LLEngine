#pragma once

#include "DrawableObject.hpp"

class TexturedDrawableObject : public DrawableObject {
public:
    GLuint texture_id;

    TexturedDrawableObject(GLuint texture_id, std::shared_ptr<Mesh> mesh);
    ~TexturedDrawableObject();
};
