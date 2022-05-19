#include "DrawableObject.hpp"

void DrawableObject::init_vertex_buf() {
    glGenBuffers(1, &vertex_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf_id);
    glBufferData(GL_ARRAY_BUFFER, vertex_buf.size() * sizeof(GLfloat), vertex_buf.data(), GL_STATIC_DRAW);
}
