#pragma once

#include "rendering/Texture.hpp"

#include <glm/mat4x4.hpp>

#include <memory>

class EquirectangularMapperShader {
public:
    EquirectangularMapperShader();

    void use_shader(const glm::mat4& mvp, const Texture& panorama_texture);

private:
    GLuint program_id = 0;
    GLint mvp_id = -1;
    GLint panorama_sampler_id = -1;
};
