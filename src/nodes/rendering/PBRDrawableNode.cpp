#include "rendering/RenderingServer.hpp"
#include "nodes/rendering/PBRDrawableNode.hpp" // PBRDrawableNode

using namespace llengine;

PBRDrawableNode::PBRDrawableNode() = default;

PBRDrawableNode::PBRDrawableNode(
    const std::shared_ptr<Material>& material,
    const std::shared_ptr<const Mesh>& mesh
) : DrawableCompleteSpatialNode(), mesh(mesh), material(material) {}

static void draw_mesh(const Mesh& mesh, RenderingServer& rs) {
    mesh.bind_vao();

    if (mesh.is_indexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.get_indices_id());
        glDrawElements(GL_TRIANGLES, mesh.get_amount_of_vertices(), mesh.get_indices_type(), 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.get_vertices_id());
        glDrawArrays(GL_TRIANGLES, 0, mesh.get_amount_of_vertices());
    }
    rs.report_about_drawn_triangles(mesh.get_amount_of_vertices() / 3);

    mesh.unbind_vao();
}

void PBRDrawableNode::draw() {
    RenderingServer& rs = get_rendering_server();

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

    draw_mesh(*mesh, rs);
}

void PBRDrawableNode::draw_to_shadow_map() {
    RenderingServer& rs = get_rendering_server();

    const glm::mat4 model_matrix = get_global_matrix();
    const glm::mat4 mvp = rs.get_dir_light_view_proj_matrix() * model_matrix;
    rs.get_shader_holder().get_shadow_mapping_shader().use_shader(mvp);

    draw_mesh(*mesh, rs);
}

GLuint PBRDrawableNode::get_program_id() const {
    RenderingServer& rs = get_rendering_server();
    return rs.get_shader_holder().get_pbr_shader_manager().get_program_id(rs, *material);
}

void PBRDrawableNode::copy_to(Node& node) const {
    CompleteSpatialNode::copy_to(node);

    PBRDrawableNode& pbr_node = dynamic_cast<PBRDrawableNode&>(node);
    pbr_node.material = material;
    pbr_node.mesh = mesh;
}

std::unique_ptr<Node> PBRDrawableNode::copy() const {
    std::unique_ptr<Node> result = std::make_unique<PBRDrawableNode>();
    copy_to(*result);
    return result;
}