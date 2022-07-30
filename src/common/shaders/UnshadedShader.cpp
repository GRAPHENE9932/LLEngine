#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "UnshadedShader.hpp"

UnshadedShader::~UnshadedShader() {
    delete_shader();
}

void UnshadedShader::initialize() {
    program_id = load_shaders(
        "res/shaders/unshaded_vertex.glsl",
        "res/shaders/unshaded_fragment.glsl"
    );

    mvp_id = glGetUniformLocation(program_id, "mvp_unif");
    color_id = glGetUniformLocation(program_id, "color_unif");
}

void UnshadedShader::use_shader(const glm::mat4& mvp, const glm::vec3& color) {
    if (!is_initialized())
        initialize();

    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(color_id, 1, glm::value_ptr(color));
}

GLuint UnshadedShader::get_program_id() {
    if (!is_initialized())
        initialize();

    return program_id;
}

void UnshadedShader::delete_shader() {
    if (is_initialized()) {
        glDeleteProgram(program_id);
        program_id = 0;
    }
}
