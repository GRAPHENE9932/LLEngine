#include <array>

#include <glm/gtx/transform.hpp>

#include "../utils/shader_loader.hpp"
#include "SkyboxObject.hpp"

constexpr std::array<float, 108> SKYBOX_VERTICES {{
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
}};

GLuint SkyboxObject::vertices_id = 0;

void SkyboxObject::pre_init() {
    // Init buffers.
    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, SKYBOX_VERTICES.size() * sizeof(float),
            SKYBOX_VERTICES.data(), GL_STATIC_DRAW);
}

void SkyboxObject::clean_up() {
    vertices_id = 0;

    glDeleteBuffers(1, &vertices_id);
}

void SkyboxObject::draw(DrawParameters& params) {
    // Uniforms.
    const glm::mat4 view_without_translation {glm::mat3(params.view_matrix)};
    const glm::mat4 mvp = params.proj_matrix * view_without_translation;
    params.sh_mgr.use_skybox_shader(mvp);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind the cubemap.
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->get_id());

    glBindVertexArray(vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, SKYBOX_VERTICES.size());
    params.triangles_drawn += SKYBOX_VERTICES.size() / 3;

    glDisableVertexAttribArray(0);
}

GLuint SkyboxObject::get_program_id(DrawParameters& params) const {
    return params.sh_mgr.get_skybox_program_id();
}
