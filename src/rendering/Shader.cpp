#include "rendering/Shader.hpp"
#include "utils/shader_loader.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace llengine::internal {
static ShaderID current_shader = 0;

static void use_shader(ShaderID shader_id) {
    if (current_shader != shader_id) {
        glUseProgram(shader_id);
        current_shader = shader_id;
    }
}

void set_int(ShaderID shader_id, ShaderUniformID uniform_id, std::int32_t value) {
    use_shader(shader_id);
    glUniform1i(uniform_id, value);
}

void set_float(ShaderID shader_id, ShaderUniformID uniform_id, float value) {
    use_shader(shader_id);
    glUniform1f(uniform_id, value);
}

void set_vec2(ShaderID shader_id, ShaderUniformID uniform_id, glm::vec2 value) {
    use_shader(shader_id);
    glUniform2fv(uniform_id, 1, glm::value_ptr(value));
}

void set_vec3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec3& value) {
    use_shader(shader_id);
    glUniform3fv(uniform_id, 1, glm::value_ptr(value));
}

void set_vec4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec4& value) {
    use_shader(shader_id);
    glUniform4fv(uniform_id, 1, glm::value_ptr(value));
}

void set_mat3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat3& value) {
    use_shader(shader_id);
    glUniformMatrix3fv(uniform_id, 1, GL_FALSE, glm::value_ptr(value));
}

void set_mat4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat4& value) {
    use_shader(shader_id);
    glUniformMatrix4fv(uniform_id, 1, GL_FALSE, glm::value_ptr(value));
}

void load_shader(
    std::string_view vertex_shader_code, std::string_view fragment_shader_code,
    const std::vector<std::string>& defines, ShaderID& id_out
) {
    id_out = load_shaders(vertex_shader_code, fragment_shader_code, defines);
}

ShaderUniformID query_uniform_id(ShaderID shader_id, std::string_view name) {
    return glGetUniformLocation(shader_id, std::string(name).c_str());
}
}