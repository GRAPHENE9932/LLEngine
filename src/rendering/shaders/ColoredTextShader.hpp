#pragma once

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace llengine {
class ColoredTextShader {
public:
    ColoredTextShader();
    ~ColoredTextShader();

    void use_shader(const glm::mat4& mvp, const glm::vec3& color);
    [[nodiscard]] GLuint get_program_id() const noexcept {
        return program_id;
    }
    void delete_shader();

private:
    GLuint program_id = 0;
    GLint mvp_id = 0, color_id = 0;
};
}