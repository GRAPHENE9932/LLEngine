#pragma once

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class ColoredTextShader {
public:
    ~ColoredTextShader();

    void initialize();
    void use_shader(const glm::mat4& mvp, const glm::vec3& color);
    GLuint get_program_id();
    void delete_shader();

    inline bool is_initialized() const noexcept {
        return program_id != 0;
    }

private:
    GLuint program_id = 0;
    GLuint mvp_id, color_id;
};
