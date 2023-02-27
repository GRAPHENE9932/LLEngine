#include "RenderingServer.hpp"
#include "PBRDrawableNode.hpp" // PBRDrawableNode

PBRDrawableNode::PBRDrawableNode(
    RenderingServer& rs,
    const std::shared_ptr<Material>& material,
    const std::shared_ptr<const Mesh>& mesh,
    const Transform& transform
) : DrawableNode(rs, transform), mesh(mesh), material(material) {}

void PBRDrawableNode::draw() {
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
    const glm::mat4 mvp = rs.get_view_proj_matrix() * model_matrix;
    rs.get_shader_holder().get_pbr_shader_manager().use_shader(
        rs, *material, mvp, model_matrix, rs.get_camera_position()
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
    rs.report_about_drawn_triangles(mesh->get_amount_of_vertices() / 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

GLuint PBRDrawableNode::get_program_id() const {
    return rs.get_shader_holder().get_pbr_shader_manager().get_program_id(rs, *material);
}
