#include <array> // std::array

#include <GL/glew.h>

#include "rendering/Skybox.hpp" // Skybox
#include "rendering/RenderingServer.hpp" // RenderingServer
#include "rendering/Mesh.hpp"

using namespace llengine;

Skybox::Skybox(Skybox&& other) noexcept :
    Skybox(std::move(other.cubemap_texture)) {}

Skybox::Skybox(const std::shared_ptr<Texture>& cubemap_texture) :
    cubemap_texture(cubemap_texture) {};

Skybox& Skybox::operator=(Skybox&& other) noexcept {
    cubemap_texture = std::move(other.cubemap_texture);
    return *this;
}

void Skybox::draw(RenderingServer& rs) {
    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    // Uniforms.
    const glm::mat4 view_without_translation = glm::mat3(rs.get_view_matrix());
    const glm::mat4 mvp = rs.get_proj_matrix() * view_without_translation;
    
    rs.get_shader_holder().get_skybox_shader().use_shader(mvp, *cubemap_texture);

    cube_mesh->bind_vao(false, false, false);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
    glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);
    rs.report_about_drawn_triangles(cube_mesh->get_amount_of_vertices() / 3);

    cube_mesh->unbind_vao();
}
