#pragma once

#include "rendering/Texture.hpp"

#include <glm/mat4x4.hpp>

#include "datatypes.hpp"

namespace llengine {
class SkyboxShader {
public:
    SkyboxShader();
    ~SkyboxShader();

    void use_shader(const glm::mat4& mvp, const Texture& cubemap_texture);
    [[nodiscard]] inline ShaderID get_program_id() const noexcept {
        return program_id;
    }

private:
    ShaderID program_id = 0;
    ShaderUniformID mvp_id = -1;
};
}