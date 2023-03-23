#pragma once

#include "common/core/Texture.hpp"

#include <glm/mat4x4.hpp>

class SpecularPrefilterShader {
public:
    SpecularPrefilterShader();

    void use_shader(
        const glm::mat4& mvp, float roughness,
        const Texture& environment_cubemap
    );

private:
    GLuint program_id = 0;
    GLint mvp_id = -1;
    GLint roughness_id = -1;
    GLint environment_cubemap_sampler_id = -1;
};
