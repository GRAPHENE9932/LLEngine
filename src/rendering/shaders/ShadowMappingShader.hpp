#pragma once

#include "datatypes.hpp"

#include <glm/mat4x4.hpp>

namespace llengine {
class ShadowMappingShader {
public:
    ShadowMappingShader();
    ~ShadowMappingShader();

    void use_shader(const glm::mat4& mvp);
    [[nodiscard]] inline ShaderID get_program_id() const noexcept {
        return program_id;
    }

private:
    ShaderID program_id = 0;
    ShaderUniformID mvp_id = -1;
};
}