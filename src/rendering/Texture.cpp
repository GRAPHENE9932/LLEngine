#include "rendering/Texture.hpp"
#include "rendering/shaders/BRDFIntegrationMapperShader.hpp"
#include "rendering/shaders/EquirectangularMapperShader.hpp"
#include "rendering/shaders/IrradiancePrecomputerShader.hpp"
#include "rendering/shaders/SpecularPrefilterShader.hpp"
#include "rendering/Mesh.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>

#include <array>

using namespace llengine;

auto draw_to_cubemap(
    glm::i32vec2 cubemap_size, std::int32_t mipmap_levels, std::invocable<const glm::mat4&, std::int32_t> auto&& drawing_function
) -> Texture {
    const glm::mat4 proj_matrix {glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f)};
    const std::array<glm::mat4, 6> mvp_matrices {
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

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
    glm::i32vec2 cur_cubemap_size = cubemap_size;
    for (std::int32_t level = 0; level < mipmap_levels; level++) {
        for (std::size_t i = 0; i < 6; i++) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, GL_RGB16F,
                cur_cubemap_size.x, cur_cubemap_size.y, 0, GL_RGB,
                GL_FLOAT, nullptr
            );
        }
        cur_cubemap_size /= 2;
    }

    // Initialize the framebuffer.
    GLuint framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    // Save the state of the viewport to restore it later.
    std::array<GLint, 4> original_viewport_params;
    glGetIntegerv(GL_VIEWPORT, original_viewport_params.data());

    // Draw!
    cur_cubemap_size = cubemap_size;
    for (std::int32_t level = 0; level < mipmap_levels; level++) {
        glViewport(0, 0, cur_cubemap_size.x, cur_cubemap_size.y);
        for (std::size_t i = 0; i < 6; i++) {
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                cubemap_id, level
            );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            drawing_function(mvp_matrices[i], level);
        }
        cur_cubemap_size /= 2;
    }
    glDeleteRenderbuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Return viewport to the original state.
    glViewport(
        original_viewport_params[0], original_viewport_params[1],
        original_viewport_params[2], original_viewport_params[3]
    );

    return Texture(cubemap_id, cubemap_size, true);
}

Texture Texture::panorama_to_cubemap(EquirectangularMapperShader& shader) const {
    if (is_cubemap()) {
        throw std::runtime_error("Unable to make cubemap from panorama because panorama is cubemap.");
    }

    const glm::u32vec2 cubemap_size {get_size().x / 2u};
    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    return draw_to_cubemap(cubemap_size, 1, [&] (const glm::mat4& mvp, std::int32_t level) {
        shader.use_shader(mvp, *this);

        cube_mesh->bind_vao(false, false, false);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao();
    });
}

constexpr glm::u32vec2 IRRADIANCE_MAP_SIZE {16u, 16u};
Texture Texture::compute_irradiance_map(IrradiancePrecomputerShader& shader) const {
    if (!is_cubemap()) {
        throw std::runtime_error("Unable to compute irradiance map because the given environment map is not cubemap.");
    }

    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.
    
    return draw_to_cubemap(IRRADIANCE_MAP_SIZE, 1, [&] (const glm::mat4& mvp, std::int32_t level) {
        shader.use_shader(mvp, *this);

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao();
    });
}

constexpr glm::u32vec2 SPECULAR_MAP_SIZE {256u, 256u};
constexpr std::int32_t SPECULAR_MAP_MIPMAP_LEVELS = 9; // log2(256) + 1. Also change LAST_PREFILTERED_MIPMAP_LEVEL in pbr_shader.frag.
Texture Texture::compute_prefiltered_specular_map(SpecularPrefilterShader& shader) const {
    if (!is_cubemap()) {
        throw std::runtime_error("Unable to compute specular map because the given environment map is not cubemap.");
    }

    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.
    
    return draw_to_cubemap(SPECULAR_MAP_SIZE, SPECULAR_MAP_MIPMAP_LEVELS, [&] (const glm::mat4& mvp, std::int32_t level) {
        float roughness = static_cast<float>(level) / (SPECULAR_MAP_MIPMAP_LEVELS - 1);
        shader.use_shader(mvp, roughness, *this);

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao();
    });
}

constexpr glm::u32vec2 BRDF_INTEGRATION_MAP_SIZE = SPECULAR_MAP_SIZE;
Texture Texture::compute_brdf_integration_map(BRDFIntegrationMapperShader& shader) {
    const auto& quad_mesh = Mesh::get_quad(); // Alias the cube.

    // Create and allocate the texture.
    GLuint texture_id {};
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RG16F, BRDF_INTEGRATION_MAP_SIZE.x, BRDF_INTEGRATION_MAP_SIZE.y,
        0, GL_RG, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize the framebuffer.
    GLuint framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    // Save the state of the viewport to restore it later.
    std::array<GLint, 4> original_viewport_params;
    glGetIntegerv(GL_VIEWPORT, original_viewport_params.data());
    // Change the viewport.
    glViewport(0, 0, BRDF_INTEGRATION_MAP_SIZE.x, BRDF_INTEGRATION_MAP_SIZE.y);

    // Draw!
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        texture_id, 0
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use_shader();

    quad_mesh->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, quad_mesh->get_amount_of_vertices());
    quad_mesh->unbind_vao();

    // Clean up.
    glDeleteRenderbuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Return viewport to the original state.
    glViewport(
        original_viewport_params[0], original_viewport_params[1],
        original_viewport_params[2], original_viewport_params[3]
    );

    return Texture(texture_id, BRDF_INTEGRATION_MAP_SIZE, false);
}

[[nodiscard]] Texture Texture::from_file(const TexLoadingParams& params) {
    if (params.file_path.ends_with(".ktx") || params.file_path.ends_with(".ktx2")) {
        return from_ktx(params);
    }
    else if (params.file_path.ends_with(".hdr")) {
        return from_rgbe(params);
    }
    else {
        throw std::runtime_error("The format of the specified texture file is unsupported or invalid.");
    }
}