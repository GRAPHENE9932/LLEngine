#pragma once

#include "rendering/Texture.hpp"

#include <glm/mat4x4.hpp>

namespace llengine {
class IrradiancePrecomputerShader {
public:
    IrradiancePrecomputerShader();

    void use_shader(
        const glm::mat4& mvp,
        const Texture& environment_cubemap
    );

private:
    GLuint program_id = 0;
    GLint mvp_id = -1;
    GLint environment_cubemap_sampler_id = -1;
};
}