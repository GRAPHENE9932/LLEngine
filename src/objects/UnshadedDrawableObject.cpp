#include "../utils/utils.hpp"
#include "UnshadedDrawableObject.hpp"

GLuint UnshadedDrawableObject::program_id;
GLuint UnshadedDrawableObject::mvp_matrix_uniform_id;
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
    color_uniform_id = glGetUniformLocation(program_id, "COLOR");
}

void UnshadedDrawableObject::clean_up() {
    glDeleteProgram(program_id);
}

void UnshadedDrawableObject::draw(const glm::mat4& vp) {
    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Uniforms.
    auto model_matrix = compute_matrix();

    auto mvp = vp * model_matrix;
    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, &mvp[0][0]);

    glUniform3fv(color_uniform_id, 1, &color[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
}

