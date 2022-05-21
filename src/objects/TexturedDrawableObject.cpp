#include "TexturedDrawableObject.hpp"

TexturedDrawableObject::TexturedDrawableObject() = default;

TexturedDrawableObject::TexturedDrawableObject(GLuint texture_id) :
    texture_id(texture_id) {

}

TexturedDrawableObject::~TexturedDrawableObject() {

}

void TexturedDrawableObject::init_normals_buf() {
    glGenBuffers(1, &normals_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, normals_buf_id);
    glBufferData(GL_ARRAY_BUFFER, normals_buf.size() * sizeof(glm::vec3), normals_buf.data(), GL_STATIC_DRAW);
}

void TexturedDrawableObject::init_uvs_buf() {
    glGenBuffers(1, &uvs_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_buf_id);
    glBufferData(GL_ARRAY_BUFFER, uvs_buf.size() * sizeof(glm::vec2), uvs_buf.data(), GL_STATIC_DRAW);
}
