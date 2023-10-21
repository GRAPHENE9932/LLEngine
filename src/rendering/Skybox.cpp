#include <array> // std::array

#include <GL/glew.h>

#include "rendering/Skybox.hpp" // Skybox
#include "rendering/RenderingServer.hpp" // RenderingServer
#include "rendering/Mesh.hpp"

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT =
    #include "shaders/skybox.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/skybox.frag"
;

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

    ensure_shader_is_initialized();
    shader->use_shader();
    shader->set_mat4<"mvp">(mvp);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture->get_id());

    cube_mesh->bind_vao(false, false, false);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
    glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

    cube_mesh->unbind_vao(false, false, false);
}

void Skybox::ensure_shader_is_initialized() {
    if (shader) {
        return;
    }

    shader = std::make_unique<ShaderType>(VERTEX_SHADER_TEXT, FRAGMENT_SHADER_TEXT);
}