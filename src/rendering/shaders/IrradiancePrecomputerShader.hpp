#pragma once

#include "rendering/Texture.hpp"
#include "datatypes.hpp"

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
    ShaderID program_id = 0;
    ShaderUniformID mvp_id = -1;
    ShaderUniformID environment_cubemap_sampler_id = -1;
};
}