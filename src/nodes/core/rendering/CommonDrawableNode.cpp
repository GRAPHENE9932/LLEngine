#include "RenderingServer.hpp"
#include "CommonDrawableNode.hpp" // CommonDrawableNode

CommonDrawableNode::CommonDrawableNode(const Transform& p,
    const std::shared_ptr<Material>& material,
    const std::shared_ptr<const Mesh>& mesh) :
    DrawableNode(p), mesh(mesh), material(material) {}

void CommonDrawableNode::draw() {
    auto& rs = RenderingServer::get_instance();

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
    PBRShaderManager::get_instance().use_common_shader(
        *material, mvp, model_matrix, rs.get_camera_position(),
        rs.get_environment_cubemap(get_global_position())
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

GLuint CommonDrawableNode::get_program_id() const {
    return PBRShaderManager::get_instance().get_common_program_id(
        *material, RenderingServer::get_instance().get_environment_cubemap(get_global_position()).has_value()
    );
}
