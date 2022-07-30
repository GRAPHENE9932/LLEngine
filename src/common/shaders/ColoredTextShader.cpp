#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "ColoredTextShader.hpp"

ColoredTextShader::~ColoredTextShader() {
    delete_shader();
}

void ColoredTextShader::initialize() {
    program_id = load_shaders(
        "res/shaders/colored_text_vertex.glsl",
        "res/shaders/colored_text_fragment.glsl"
    );

    mvp_id = glGetUniformLocation(program_id, "mvp_unif");
    color_id = glGetUniformLocation(program_id, "color_unif");
}

void ColoredTextShader::use_shader(const glm::mat4& mvp, const glm::vec3& color) {
    if (!is_initialized())
        initialize();

    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(color_id, 1, glm::value_ptr(color));
}

GLuint ColoredTextShader::get_program_id() {
    if (!is_initialized())
        initialize();

    return program_id;
}

void ColoredTextShader::delete_shader() {
    if (is_initialized()) {
        glDeleteShader(program_id);
        program_id = 0;
    }
}
