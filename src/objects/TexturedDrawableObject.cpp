#include "TexturedDrawableObject.hpp"

GLuint TexturedDrawableObject::program_id;
GLuint TexturedDrawableObject::mvp_matrix_uniform_id;
GLuint TexturedDrawableObject::object_matrix_uniform_id;
GLuint TexturedDrawableObject::camera_matrix_uniform_id;
GLuint TexturedDrawableObject::light_direction_uniform_id;

TexturedDrawableObject::TexturedDrawableObject(std::shared_ptr<utils::ManagedTextureID> texture_id,
    std::shared_ptr<Mesh> mesh) :
    texture_id(texture_id) {
    this->mesh = mesh;
}

TexturedDrawableObject::~TexturedDrawableObject() {

}

void TexturedDrawableObject::pre_init() {
    // Init shaders.
    program_id = utils::load_shaders("res/shaders/textured_vertex.glsl",
                                     "res/shaders/textured_fragment.glsl");
    mvp_matrix_uniform_id = glGetUniformLocation(program_id, "MVP");
    object_matrix_uniform_id = glGetUniformLocation(program_id, "OBJECT_MATRIX");
    camera_matrix_uniform_id = glGetUniformLocation(program_id, "CAMERA_MATRIX");
    light_direction_uniform_id = glGetUniformLocation(program_id, "LIGHT_DIRECTION");
}

void TexturedDrawableObject::clean_up() {
    glDeleteProgram(program_id);
}

void TexturedDrawableObject::draw(GLfloat* camera_mvp, GLfloat* view_matrix, GLfloat* light_direction) {
    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals.
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normals_id);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture_id->texture_id);

    // Uniforms.
    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, camera_mvp);
    auto obj_matrix = compute_matrix();
    glUniformMatrix4fv(object_matrix_uniform_id, 1, GL_FALSE, &obj_matrix[0][0]);
    glUniformMatrix4fv(camera_matrix_uniform_id, 1, GL_FALSE, view_matrix);
    glUniform3fv(light_direction_uniform_id, 1, light_direction);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
