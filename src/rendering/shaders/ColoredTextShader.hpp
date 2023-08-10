#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "datatypes.hpp"

namespace llengine {
class ColoredTextShader {
public:
    ColoredTextShader();
    ~ColoredTextShader();

    void use_shader(const glm::mat4& mvp, const glm::vec3& color);
    [[nodiscard]] ShaderID get_program_id() const noexcept {
        return program_id;
    }
    void delete_shader();

private:
    ShaderID program_id = 0;
    ShaderUniformID mvp_id = -1, color_id = -1;
};
}