#include "RenderingServer.hpp"
#include "CommonDrawableNode.hpp" // CommonDrawableNode

CommonDrawableNode::CommonDrawableNode(const Transform& p, RenderingServer& rs,
    const std::shared_ptr<Material>& material,
    const std::shared_ptr<IMesh>& mesh) :
    DrawableNode(p, rs), mesh(mesh), material(material) {}

void CommonDrawableNode::draw() {
    // Do some checks.
    if (material->normal_map.has_value() &&
        (mesh->get_normals_id() == 0 || mesh->get_tangents_id() == 0)) {
        throw std::runtime_error(
            "Drawable node's material has a normal map, but "
            "its mesh doesn't have normals and/or tangents."
        );
    }

    // Use the shader.
    const glm::mat4 model_matrix = get_global_matrix();
    const glm::mat4 mvp = rendering_server.get_view_proj_matrix() * model_matrix;
    rendering_server.get_shader_manager().use_common_shader(
        *material, mvp, model_matrix
    );

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_uvs_id());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals.
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normals_id());
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Tangents.
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_tangents_id());
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);

    if (mesh->is_indexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, mesh->get_amount_of_vertices(), mesh->get_indices_type(), 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
        glDrawArrays(GL_TRIANGLES, 0, mesh->get_amount_of_vertices());
    }
    rendering_server.report_about_drawn_triangles(mesh->get_amount_of_vertices() / 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

GLuint CommonDrawableNode::get_program_id() const {
    return rendering_server.get_shader_manager().get_common_program_id(
        *material
    );
}
