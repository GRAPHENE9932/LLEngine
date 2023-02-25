#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "SkyboxShader.hpp"

SkyboxShader::SkyboxShader() {
    program_id = load_shaders(
        "res/shaders/skybox.vert",
        "res/shaders/skybox.frag"
    );

    mvp_id = glGetUniformLocation(program_id, "mvp");
}

SkyboxShader::~SkyboxShader() {
    glDeleteProgram(program_id);
    program_id = 0;
}

void SkyboxShader::use_shader(const glm::mat4& mvp) {
    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
}
