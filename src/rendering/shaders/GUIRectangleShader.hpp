#pragma once

#include "rendering/Texture.hpp"

#include <glm/mat4x4.hpp>

#include "datatypes.hpp"

namespace llengine {
class GUIRectangleShader {
public:
    GUIRectangleShader();

    void use_shader(
        const Texture& texture, const glm::mat4& mvp, glm::vec2 uv_scale,
        glm::vec2 uv_offset, const glm::vec4& color_factor
    );

private:
    ShaderID program_id = 0;
    ShaderUniformID mvp_id = -1;
    ShaderUniformID uv_scale_id = -1;
    ShaderUniformID uv_offset_id = -1;
    ShaderUniformID color_factor_id = -1;
    ShaderUniformID sampler_id = -1;
};
}