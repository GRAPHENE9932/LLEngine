#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "ColoredTextShader.hpp"

ColoredTextShader::ColoredTextShader() {
    program_id = load_shaders(
        "res/shaders/colored_text.vert",
        "res/shaders/colored_text.frag"
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
