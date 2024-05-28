#include "texture_utils.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Shader.hpp"
#include "rendering/LazyShader.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/RenderingServer.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <GL/glew.h>

namespace llengine::tex_utils {
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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmap_levels - 1);
    glm::i32vec2 cur_cubemap_size = cubemap_size;
    for (std::int32_t level = 0; level < mipmap_levels; level++) {
        for (std::size_t i = 0; i < 6; i++) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GraphicsAPIEnum>(i), level, GL_RGB16F,
                cur_cubemap_size.x, cur_cubemap_size.y, 0, GL_RGB,
                GL_FLOAT, nullptr
            );
        }
        cur_cubemap_size /= 2;
    }

    // Initialize the framebuffer.
    ManagedFramebufferID framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id.get_ref());
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
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GraphicsAPIEnum>(i),
                cubemap_id, level
            );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            drawing_function(mvp_matrices[i], level);
        }
        cur_cubemap_size /= 2;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, RenderingServer::get_current_default_framebuffer_id());

    // Return viewport to the original state.
    glViewport(
        original_viewport_params[0], original_viewport_params[1],
        original_viewport_params[2], original_viewport_params[3]
    );

    return Texture::from_texture_id(cubemap_id, cubemap_size, Texture::Type::TEX_CUBEMAP);
}

constexpr std::string_view EQUIRECTANGULAR_MAPPER_VERTEX_SHADER_TEXT =
    #include "shaders/misc/equirectangular_mapper.vert"
;
constexpr std::string_view EQUIRECTANGULAR_MAPPER_FRAGMENT_SHADER_TEXT =
    #include "shaders/misc/equirectangular_mapper.frag"
;
static LazyShader<Shader<"mvp">> equirectangular_mapper_shader {EQUIRECTANGULAR_MAPPER_VERTEX_SHADER_TEXT, EQUIRECTANGULAR_MAPPER_FRAGMENT_SHADER_TEXT};

[[nodiscard]] Texture panorama_to_cubemap(const Texture& panorama) {
    if (panorama.get_type() != Texture::Type::TEX_2D) {
        throw std::runtime_error("Unable to make cubemap from panorama because panorama has invalid texture type.");
    }

    const glm::u32vec2 cubemap_size {panorama.get_size().x / 2u};
    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    equirectangular_mapper_shader->use_shader();
    return draw_to_cubemap(cubemap_size, 1, [&] (const glm::mat4& mvp, std::int32_t level) {
        equirectangular_mapper_shader->set_mat4<"mvp">(mvp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, panorama.get_id());

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao(false, false, false);
    });
}

constexpr std::string_view IRRADIANCE_PRECOMPUTER_VERTEX_SHADER_TEXT =
    #include "shaders/objects/pbr/irradiance_precomputer.vert"
;
constexpr std::string_view IRRADIANCE_PRECOMPUTER_FRAGMENT_SHADER_TEXT =
    #include "shaders/objects/pbr/irradiance_precomputer.frag"
;
static LazyShader<Shader<"mvp">> irradiance_precomputer_shader {IRRADIANCE_PRECOMPUTER_VERTEX_SHADER_TEXT, IRRADIANCE_PRECOMPUTER_FRAGMENT_SHADER_TEXT};

constexpr glm::u32vec2 IRRADIANCE_MAP_SIZE {16u, 16u};

[[nodiscard]] Texture compute_irradiance_map(const Texture& cubemap) {
    if (cubemap.get_type() != Texture::Type::TEX_CUBEMAP) {
        throw std::runtime_error("Unable to compute irradiance map because the given environment map is not cubemap.");
    }

    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    irradiance_precomputer_shader->use_shader();
    return draw_to_cubemap(IRRADIANCE_MAP_SIZE, 1, [&] (const glm::mat4& mvp, std::int32_t level) {
        irradiance_precomputer_shader->set_mat4<"mvp">(mvp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.get_id());

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao(false, false, false);
    });
}

constexpr std::string_view SPECULAR_PREFILTER_VERTEX_SHADER_TEXT =
    #include "shaders/objects/pbr/specular_prefilter.vert"
;
constexpr std::string_view SPECULAR_PREFILTER_FRAGMENT_SHADER_TEXT =
    #include "shaders/objects/pbr/specular_prefilter.frag"
;
static LazyShader<Shader<"mvp", "roughness">> specular_prefilter_shader {SPECULAR_PREFILTER_VERTEX_SHADER_TEXT, SPECULAR_PREFILTER_FRAGMENT_SHADER_TEXT};

constexpr glm::u32vec2 SPECULAR_MAP_SIZE {256u, 256u};
constexpr std::int32_t SPECULAR_MAP_MIPMAP_LEVELS = 9; // log2(256) + 1. Also change LAST_PREFILTERED_MIPMAP_LEVEL in pbr_shader.frag.

[[nodiscard]] Texture compute_prefiltered_specular_map(const Texture& cubemap) {
    if (cubemap.get_type() != Texture::Type::TEX_CUBEMAP) {
        throw std::runtime_error("Unable to compute specular map because the given environment map is not cubemap.");
    }

    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    specular_prefilter_shader->use_shader();
    return draw_to_cubemap(SPECULAR_MAP_SIZE, SPECULAR_MAP_MIPMAP_LEVELS, [&] (const glm::mat4& mvp, std::int32_t level) {
        float roughness = static_cast<float>(level) / (SPECULAR_MAP_MIPMAP_LEVELS - 1);
        specular_prefilter_shader->set_mat4<"mvp">(mvp);
        specular_prefilter_shader->set_float<"roughness">(roughness);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.get_id());

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao(false, false, false);
    });
}

constexpr std::string_view BRDF_INTEGRATION_MAPPER_VERTEX_SHADER_TEXT =
    #include "shaders/objects/pbr/brdf_integration_mapper.vert"
;
constexpr std::string_view BRDF_INTEGRATION_MAPPER_FRAGMENT_SHADER_TEXT =
    #include "shaders/objects/pbr/brdf_integration_mapper.frag"
;
static LazyShader<Shader<>> brdf_integration_mapper_shader {BRDF_INTEGRATION_MAPPER_VERTEX_SHADER_TEXT, BRDF_INTEGRATION_MAPPER_FRAGMENT_SHADER_TEXT};

constexpr glm::u32vec2 BRDF_INTEGRATION_MAP_SIZE = SPECULAR_MAP_SIZE;

[[nodiscard]] Texture compute_brdf_integration_map() {
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
    ManagedFramebufferID framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id.get_ref());
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

    brdf_integration_mapper_shader->use_shader();

    glClear(GL_COLOR_BUFFER_BIT);
    quad_mesh->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, quad_mesh->get_amount_of_vertices());
    quad_mesh->unbind_vao(true, false, false);

    // Clean up.
    glBindFramebuffer(GL_FRAMEBUFFER, RenderingServer::get_current_default_framebuffer_id());

    // Return viewport to the original state.
    glViewport(
        original_viewport_params[0], original_viewport_params[1],
        original_viewport_params[2], original_viewport_params[3]
    );

    return Texture::from_texture_id(texture_id, BRDF_INTEGRATION_MAP_SIZE, Texture::Type::TEX_2D);
}
}
