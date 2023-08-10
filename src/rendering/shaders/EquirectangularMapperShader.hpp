#pragma once

#include "rendering/Texture.hpp"
#include "datatypes.hpp"

#include <glm/mat4x4.hpp>

#include <memory>

namespace llengine {
class EquirectangularMapperShader {
public:
    EquirectangularMapperShader();

    void use_shader(const glm::mat4& mvp, const Texture& panorama_texture);

private:
    ShaderID program_id = 0;
    ShaderUniformID mvp_id = -1;
    ShaderUniformID panorama_sampler_id = -1;
};
}