#include <array> // std::array

#include "Skybox.hpp" // Skybox
#include "RenderingServer.hpp" // RenderingServer
#include "utils/primitive_meshes.hpp"

Skybox::Skybox(Skybox&& other) noexcept :
        Skybox(other.rs, other.cubemap_texture) {
    other.cubemap_texture = nullptr;
}

Skybox::Skybox(RenderingServer& rs, const std::shared_ptr<Texture>& cubemap_texture) :
    cubemap_texture(cubemap_texture), rs(rs) {};

void Skybox::draw() {
    const auto& cube_mesh = primitives::get_skybox_cube(); // Alias the cube.

    // Uniforms.
    const glm::mat4 view_without_translation = glm::mat3(rs.get_view_matrix());
    const glm::mat4 mvp = rs.get_proj_matrix() * view_without_translation;
    
    rs.get_shader_holder().get_skybox_shader().use_shader(mvp, *cubemap_texture);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, cube_mesh->get_vertices_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
    glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);
    rs.report_about_drawn_triangles(cube_mesh->get_amount_of_vertices() / 3);

    glDisableVertexAttribArray(0);
}
