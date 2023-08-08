#include "ShadowMappingShader.hpp"
#include "utils/shader_loader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string_view>

namespace llengine {
constexpr std::string_view VERTEX_SHADER_TEXT = 
    #include "shaders/shadow_mapping.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/shadow_mapping.frag"
;

ShadowMappingShader::ShadowMappingShader() {
    program_id = load_shaders(
        VERTEX_SHADER_TEXT,
        FRAGMENT_SHADER_TEXT
    );

    mvp_id = glGetUniformLocation(program_id, "mvp");
}

ShadowMappingShader::~ShadowMappingShader() {
    glDeleteProgram(program_id);
    program_id = 0;
}

void ShadowMappingShader::use_shader(const glm::mat4 &mvp) {
    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
}
}