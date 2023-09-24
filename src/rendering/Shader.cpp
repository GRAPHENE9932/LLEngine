#include "rendering/Shader.hpp"
#include "utils/shader_loader.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace llengine::internal {
void use_shader(ShaderID shader_id) {
    glUseProgram(shader_id);
}

void set_int(ShaderID shader_id, ShaderUniformID uniform_id, std::int32_t value) {
    glUniform1i(uniform_id, value);
}

void set_uint(ShaderID shader_id, ShaderUniformID uniform_id, std::uint32_t value) {
    glUniform1ui(uniform_id, value);
}

void set_float(ShaderID shader_id, ShaderUniformID uniform_id, float value) {
    glUniform1f(uniform_id, value);
}

void set_vec2(ShaderID shader_id, ShaderUniformID uniform_id, glm::vec2 value) {
    glUniform2fv(uniform_id, 1, glm::value_ptr(value));
}

void set_vec3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec3& value) {
    glUniform3fv(uniform_id, 1, glm::value_ptr(value));
}

void set_vec4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec4& value) {
    glUniform4fv(uniform_id, 1, glm::value_ptr(value));
}

void set_mat3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat3& value) {
    glUniformMatrix3fv(uniform_id, 1, GL_FALSE, glm::value_ptr(value));
}

void set_mat4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat4& value) {
    glUniformMatrix4fv(uniform_id, 1, GL_FALSE, glm::value_ptr(value));
}

static void bind_texture(ShaderUniformID uniform_id, GraphicsAPIEnum unit, TextureID texture_id, GraphicsAPIEnum target) {
    glUniform1i(uniform_id, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, texture_id);
}

void bind_2d_texture(ShaderUniformID uniform_id, GraphicsAPIEnum unit, TextureID texture_id) {
    bind_texture(uniform_id, unit, texture_id, GL_TEXTURE_2D);
}

void bind_cubemap_texture(ShaderUniformID uniform_id, GraphicsAPIEnum unit, TextureID texture_id) {
    bind_texture(uniform_id, unit, texture_id, GL_TEXTURE_CUBE_MAP);
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