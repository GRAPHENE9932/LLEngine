#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "SkyboxShader.hpp"

SkyboxShader::~SkyboxShader() {
    delete_shader();
}

void SkyboxShader::initialize() {
    program_id = load_shaders(
        "res/shaders/skybox_vertex.glsl",
        "res/shaders/skybox_fragment.glsl"
    );

    mvp_id = glGetUniformLocation(program_id, "MVP");
}

void SkyboxShader::use_shader(const glm::mat4& mvp) {
    if (!is_initialized())
        initialize();

    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
}

GLuint SkyboxShader::get_program_id() {
    if (!is_initialized())
        initialize();

    return program_id;
}

void SkyboxShader::delete_shader() {
    if (is_initialized()) {
        glDeleteProgram(program_id);
        program_id = 0;
    }
}