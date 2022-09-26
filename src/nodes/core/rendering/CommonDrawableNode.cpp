#include "CommonDrawableNode.hpp"
#include "utils/shader_loader.hpp"
#include "SceneTree.hpp" // SceneTree

CommonDrawableNode::CommonDrawableNode(const SpatialParams& params,
                                           SceneTree& scene_tree,
                                           const std::shared_ptr<Material>& material,
                                           const std::shared_ptr<IMesh>& mesh) :
                                           DrawableNode(params, scene_tree),
                                           mesh(mesh), material(material) {}

void CommonDrawableNode::draw() {
    const auto& context = scene_tree.get_context();

    // Use the shader.
    const glm::mat4 model_matrix = get_global_matrix();
    const glm::mat4 mvp = context.view_proj_matrix * model_matrix;
    scene_tree.get_shader_manager().use_common_shader(
        *material, context, mvp, model_matrix
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
    scene_tree.report_about_drawn_triangles(mesh->get_amount_of_vertices() / 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

void CommonDrawableNode::register_myself(SpatialNode *parent) {
    scene_tree.register_node(this, parent);
}

GLuint CommonDrawableNode::get_program_id() const {
    return scene_tree.get_shader_manager().get_common_program_id(
        *material, scene_tree.get_context()
    );
}
