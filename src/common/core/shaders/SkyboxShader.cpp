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

void SkyboxShader::use_shader(const glm::mat4& mvp, const Texture& cubemap_texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture.get_id());

    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
}
