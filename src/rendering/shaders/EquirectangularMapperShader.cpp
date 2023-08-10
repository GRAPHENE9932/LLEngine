#include "EquirectangularMapperShader.hpp"
#include "utils/shader_loader.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT = 
    #include "shaders/equirectangular_mapper.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/equirectangular_mapper.frag"
;

EquirectangularMapperShader::EquirectangularMapperShader() {
    program_id = load_shaders(
        VERTEX_SHADER_TEXT,
        FRAGMENT_SHADER_TEXT
    );

    mvp_id = glGetUniformLocation(program_id, "mvp");
    panorama_sampler_id = glGetUniformLocation(program_id, "equirectangular_map");
}

void EquirectangularMapperShader::use_shader(
    const glm::mat4& mvp,
    const Texture& panorama_texture
) { 
    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(panorama_sampler_id, 0);
    glActiveTexture(GL_TEXTURE0);
    
    glBindTexture(GL_TEXTURE_2D, panorama_texture.get_id());
}
