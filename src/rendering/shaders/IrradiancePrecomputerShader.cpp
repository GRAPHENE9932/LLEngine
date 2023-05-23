#include "IrradiancePrecomputerShader.hpp"
#include "utils/shader_loader.hpp"

#include <glm/gtc/type_ptr.hpp>

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT = 
    #include "shaders/irradiance_precomputer.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/irradiance_precomputer.frag"
;

IrradiancePrecomputerShader::IrradiancePrecomputerShader() {
    program_id = load_shaders(
        VERTEX_SHADER_TEXT,
        FRAGMENT_SHADER_TEXT
    );

    mvp_id = glGetUniformLocation(program_id, "mvp");
    environment_cubemap_sampler_id = glGetUniformLocation(program_id, "cubemap");
}

void IrradiancePrecomputerShader::use_shader(
    const glm::mat4& mvp,
    const Texture& environment_cubemap
) {
    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(environment_cubemap_sampler_id, 0);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap.get_id());
}
