#include "../utils/utils.hpp"
#include "UnshadedDrawableObject.hpp"

GLuint UnshadedDrawableObject::program_id;
GLuint UnshadedDrawableObject::mvp_matrix_uniform_id;
GLuint UnshadedDrawableObject::model_matrix_uniform_id;
GLuint UnshadedDrawableObject::color_uniform_id;

UnshadedDrawableObject::UnshadedDrawableObject(std::shared_ptr<Mesh> mesh, glm::vec3 color) :
    color(color) {
    this->mesh = mesh;
}

UnshadedDrawableObject::~UnshadedDrawableObject() {

}

void UnshadedDrawableObject::pre_init() {
    // Init shaders.
    program_id = utils::load_shaders("res/shaders/unshaded_vertex.glsl",
                                     "res/shaders/unshaded_fragment.glsl");
    // Init uniforms.
    mvp_matrix_uniform_id = glGetUniformLocation(program_id, "MVP");
    model_matrix_uniform_id = glGetUniformLocation(program_id, "MODEL_MATRIX");
    color_uniform_id = glGetUniformLocation(program_id, "COLOR");
}

void UnshadedDrawableObject::clean_up() {
    glDeleteProgram(program_id);
}

void UnshadedDrawableObject::draw(GLfloat* camera_mvp) {
    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Uniforms.
    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, camera_mvp);
    auto model_matrix = compute_matrix();
    glUniformMatrix4fv(model_matrix_uniform_id, 1, GL_FALSE, &model_matrix[0][0]);
    glUniform3fv(color_uniform_id, 1, &color[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
}

