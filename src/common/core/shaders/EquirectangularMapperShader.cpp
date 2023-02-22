#include "EquirectangularMapperShader.hpp"
#include "utils/shader_loader.hpp"

#include <glm/gtc/type_ptr.hpp>

void EquirectangularMapperShader::use_shader(
    const glm::mat4& mvp,
    GLuint panorama_texture_id
) {
    if (program_id == 0) {
        initialize_shader();
    }
    
    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(panorama_sampler_id, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, panorama_texture_id);
}

void EquirectangularMapperShader::initialize_shader() {
    program_id = load_shaders(
        "res/shaders/equirectangular_mapper.vert",
        "res/shaders/equirectangular_mapper.frag"
    );

    mvp_id = glGetUniformLocation(program_id, "mvp");
    panorama_sampler_id = glGetUniformLocation(program_id, "equirectangular_map");
}