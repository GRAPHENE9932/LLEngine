#pragma once

#include "common/core/Texture.hpp"

#include <glm/mat4x4.hpp>

#include <memory>

class EquirectangularMapperShader {
public:
    EquirectangularMapperShader() = default;
    void use_shader(const glm::mat4& mvp, GLuint panorama_texture_id);

private:
    GLuint program_id = 0;
    GLint mvp_id = -1;
    GLint panorama_sampler_id = -1;

    void initialize_shader();
};