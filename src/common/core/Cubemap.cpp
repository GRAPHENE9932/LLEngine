#include <array> // std::array

#include "Cubemap.hpp" // Skybox
#include "RenderingServer.hpp" // RenderingServer
#include "utils/primitive_meshes.hpp"

Cubemap::Cubemap(Cubemap&& other) noexcept :
        Cubemap(other.cubemap_texture) {
    other.cubemap_texture = nullptr;
}

Cubemap::Cubemap(const std::shared_ptr<Texture>& cubemap_texture) :
    cubemap_texture(cubemap_texture) {};

Cubemap Cubemap::from_cubemap(const std::shared_ptr<Texture>& cubemap_texture) {
    return Cubemap(cubemap_texture);
}

Cubemap Cubemap::from_panorama(const std::shared_ptr<Texture>& panorama_texture) {
    const glm::mat4 proj_matrix {glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f)};
    const std::array<glm::mat4, 6> mvp_matrices {
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    glm::u32vec2 cube_map_size {panorama_texture->get_size().x / 2u};

    // Allocate the cube map.
    GLuint cubemap_id;
    glActiveTexture(GL_TEXTURE1); // GL_TEXTURE0 will be used for the shader.
    glGenTextures(1, &cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (std::size_t i = 0; i < 6; i++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            cube_map_size.x, cube_map_size.y, 0, GL_RGB, 
            GL_FLOAT, nullptr
        );
    }
    
    // Initialize the framebuffer.
    GLuint framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cube_map_size.x, cube_map_size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // Render.
    glViewport(0, 0, cube_map_size.x, cube_map_size.y);
    const auto& cube_mesh = primitives::get_skybox_cube(); // Alias the cube.
    for (std::size_t i = 0; i < 6; i++) {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            cubemap_id, 0
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderingServer::get_instance().get_shader_manager()
                .use_equirectangular_mapper_shader(mvp_matrices[i], panorama_texture->get_id());
        
        // Draw.
        // Vertices.
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, cube_mesh->get_vertices_id());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // Bind the cubemap.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);
        RenderingServer::get_instance().report_about_drawn_triangles(cube_mesh->get_amount_of_vertices() / 3);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1500, 800);

    return Cubemap::from_cubemap(std::make_shared<Texture>(cubemap_id, cube_map_size, true));
}

void Cubemap::draw() {
    auto& rs = RenderingServer::get_instance(); // Alias the rendering server;
    const auto& cube_mesh = primitives::get_skybox_cube(); // Alias the cube.

    // Uniforms.
    const glm::mat4 view_without_translation = glm::mat3(rs.get_view_matrix());
    const glm::mat4 mvp = rs.get_proj_matrix() * view_without_translation;
    
    rs.get_shader_manager().use_skybox_shader(mvp);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, cube_mesh->get_vertices_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind the cubemap.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture->get_id());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
    glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);
    rs.report_about_drawn_triangles(cube_mesh->get_amount_of_vertices() / 3);

    glDisableVertexAttribArray(0);
}
