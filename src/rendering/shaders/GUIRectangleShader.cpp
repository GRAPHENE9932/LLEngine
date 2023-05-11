#include "GUIRectangleShader.hpp"
#include "utils/shader_loader.hpp"
#include "rendering/Texture.hpp"

#include <glm/gtc/type_ptr.inl>

GUIRectangleShader::GUIRectangleShader() {
    program_id = load_shaders(
        "res/shaders/gui_rectangle.vert",
        "res/shaders/gui_rectangle.frag"
    );

    mvp_id = glGetUniformLocation(program_id, "mvp");
    uv_scale_id = glGetUniformLocation(program_id, "uv_scale");
    uv_offset_id = glGetUniformLocation(program_id, "uv_offset");
    color_factor_id = glGetUniformLocation(program_id, "color_factor");
    sampler_id = glGetUniformLocation(program_id, "base_color_texture");
}

void GUIRectangleShader::use_shader(
    const Texture& texture, const glm::mat4& mvp, glm::vec2 uv_scale,
    glm::vec2 uv_offset, const glm::vec4& color_factor
) {
    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform2fv(uv_scale_id, 1, glm::value_ptr(uv_scale));
    glUniform2fv(uv_offset_id, 1, glm::value_ptr(uv_offset));
    glUniform4fv(color_factor_id, 1, glm::value_ptr(color_factor));
    glUniform1i(sampler_id, 0);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture.get_id());
}
