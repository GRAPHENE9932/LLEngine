#include "texture_tools.hpp"
#include "primitive_meshes.hpp"
#include "RenderingServer.hpp"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

#include <array>

std::shared_ptr<Texture> panorama_to_cubemap(const Texture& panorama) {
    if (panorama.is_cubemap()) {
        throw std::runtime_error("Unable to make cubemap from panorama because panorama is cubemap.");
    }

    const glm::mat4 proj_matrix {glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f)};
    const std::array<glm::mat4, 6> mvp_matrices {
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    glm::u32vec2 cubemap_size {panorama.get_size().x / 2u};

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
            cubemap_size.x, cubemap_size.y, 0, GL_RGB, 
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cubemap_size.x, cubemap_size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // Save the state of the viewport to restore it later.
    std::array<GLint, 4> original_viewport_params;
    glGetIntegerv(GL_VIEWPORT, original_viewport_params.data());

    glViewport(0, 0, cubemap_size.x, cubemap_size.y);
    const auto& cube_mesh = primitives::get_skybox_cube(); // Alias the cube.
    for (std::size_t i = 0; i < 6; i++) {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            cubemap_id, 0
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        EquirectangularMapperShader::get_instance().use_shader(mvp_matrices[i], panorama.get_id());
        
        // Draw.
        // Vertices.
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, cube_mesh->get_vertices_id());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // Bind the cubemap.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Return viewport to the original state.
    glViewport(
        original_viewport_params[0], original_viewport_params[1],
        original_viewport_params[2], original_viewport_params[3]
    );

    return std::make_shared<Texture>(cubemap_id, cubemap_size, true);
}
