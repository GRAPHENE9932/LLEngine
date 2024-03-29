#include <array> // std::array

#include <GL/glew.h>

#include "rendering/Skybox.hpp" // Skybox
#include "rendering/RenderingServer.hpp" // RenderingServer
#include "rendering/Mesh.hpp"
#include "nodes/rendering/CameraNode.hpp"

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT =
    #include "shaders/skybox.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/skybox.frag"
;

LazyShader<Skybox::ShaderType> Skybox::shader = {VERTEX_SHADER_TEXT, FRAGMENT_SHADER_TEXT};

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
    const glm::mat4 view_without_translation = glm::mat3(rs.get_current_camera_node().get_view_matrix());
    const glm::mat4 mvp = rs.get_current_camera_node().get_proj_matrix() * view_without_translation;

    shader->use_shader();
    shader->set_mat4<"mvp">(mvp);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture->get_id());

    cube_mesh->bind_vao(false, false, false);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
    glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

    cube_mesh->unbind_vao(false, false, false);
}
