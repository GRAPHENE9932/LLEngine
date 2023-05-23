#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "ColoredTextShader.hpp"

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT = 
    #include "shaders/colored_text.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/colored_text.frag"
;

ColoredTextShader::ColoredTextShader() {
    program_id = load_shaders(
        VERTEX_SHADER_TEXT,
        FRAGMENT_SHADER_TEXT
    );

    mvp_id = glGetUniformLocation(program_id, "mvp");
    color_id = glGetUniformLocation(program_id, "text_color");
}

ColoredTextShader::~ColoredTextShader() {
    glDeleteShader(program_id);
    program_id = 0;
}

void ColoredTextShader::use_shader(const glm::mat4& mvp, const glm::vec3& color) {
    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(color_id, 1, glm::value_ptr(color));
}
