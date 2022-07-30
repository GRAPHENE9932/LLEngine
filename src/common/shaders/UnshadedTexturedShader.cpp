#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "UnshadedTexturedShader.hpp"

UnshadedTexturedShader::~UnshadedTexturedShader() {
    delete_shader();
}

void UnshadedTexturedShader::initialize() {
    program_id = load_shaders(
        "res/shaders/unshaded_textured_vertex.glsl",
        "res/shaders/unshaded_textured_fragment.glsl"
    );

    mvp_id = glGetUniformLocation(program_id, "mvp_unif");
}

void UnshadedTexturedShader::use_shader(const glm::mat4& mvp) {
    if (!is_initialized())
        initialize();

    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
}

GLuint UnshadedTexturedShader::get_program_id() {
    if (!is_initialized())
        initialize();

    return program_id;
}

void UnshadedTexturedShader::delete_shader() {
    if (is_initialized()) {
        glDeleteProgram(program_id);
        program_id = 0;
    }
}
