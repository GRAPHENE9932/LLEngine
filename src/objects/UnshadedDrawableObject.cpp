#include "UnshadedDrawableObject.hpp"
#include "../utils/shader_loader.hpp"

UnshadedDrawableObject::UnshadedDrawableObject(std::shared_ptr<Mesh> mesh, glm::vec3 color) :
    color(color) {
    this->mesh = mesh;
}

void UnshadedDrawableObject::draw(DrawParameters& params) {
    // Uniforms.
    auto model_matrix = compute_matrix();
    auto mvp = params.view_proj_matrix * model_matrix;
    params.sh_mgr.use_unshaded_shader(mvp, color);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_indices_id());

    glDrawElements(GL_TRIANGLES, mesh->get_indices().size(), GL_UNSIGNED_SHORT, 0);
    params.triangles_drawn += mesh->get_indices().size() / 3;

    glDisableVertexAttribArray(0);
}

GLuint UnshadedDrawableObject::get_program_id(DrawParameters& params) const {
    return params.sh_mgr.get_unshaded_program_id();
}
