#pragma once

#include "rendering/Texture.hpp"

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

namespace llengine {
class GUIRectangleShader {
public:
    GUIRectangleShader();

    void use_shader(
        const Texture& texture, const glm::mat4& mvp, glm::vec2 uv_scale,
        glm::vec2 uv_offset, const glm::vec4& color_factor
    );

private:
    GLuint program_id = 0;
    GLint mvp_id = -1;
    GLint uv_scale_id = -1;
    GLint uv_offset_id = -1;
    GLint color_factor_id = -1;
    GLint sampler_id = -1;
};
}