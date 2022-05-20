#include "MonochromeDrawableObject.hpp"

MonochromeDrawableObject::MonochromeDrawableObject() = default;

MonochromeDrawableObject::MonochromeDrawableObject(glm::vec3 color) : color(color) {}

void MonochromeDrawableObject::init_normals_buf() {
    glGenBuffers(1, &normals_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, normals_buf_id);
    glBufferData(GL_ARRAY_BUFFER, normals_buf.size() * sizeof(glm::vec3), normals_buf.data(), GL_STATIC_DRAW);
}
