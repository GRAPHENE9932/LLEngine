#include "../utils/shader_loader.hpp"
#include "SkyboxObject.hpp"

#include <glm/gtx/transform.hpp>

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

GLuint SkyboxObject::program_id = 0;
GLuint SkyboxObject::vertices_id = 0;
GLuint SkyboxObject::mvp_uniform_id = 0;
GLuint SkyboxObject::cubemap_uniform_id = 0;

void SkyboxObject::pre_init() {
    // Init shaders.
    program_id = load_shaders(
        "res/shaders/skybox_vertex.glsl",
        "res/shaders/skybox_fragment.glsl"
    );

    // Init uniforms.
    mvp_uniform_id = glGetUniformLocation(program_id, "MVP");
    cubemap_uniform_id = glGetUniformLocation(program_id, "CUBEMAP_SAMPLER");

    // Init buffers.
    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, SKYBOX_VERTICES.size() * sizeof(float),
            SKYBOX_VERTICES.data(), GL_STATIC_DRAW);
}

void SkyboxObject::clean_up() {
    glDeleteProgram(program_id);
}

void SkyboxObject::draw(DrawParameters& params) {
    if (program_id == 0)
        pre_init();

    if (params.cur_shader != program_id) {
        params.cur_shader = program_id;
        glUseProgram(program_id);
    }

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind the cubemap.
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->get_id());

    // Uniforms.
    const glm::mat4 view_without_translation {glm::mat3(params.view_matrix)};
    glm::mat4 mvp = params.proj_matrix * view_without_translation;
    glUniformMatrix4fv(mvp_uniform_id, 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, SKYBOX_VERTICES.size());
    params.triangles_drawn += SKYBOX_VERTICES.size() / 3;

    glDisableVertexAttribArray(0);
}
