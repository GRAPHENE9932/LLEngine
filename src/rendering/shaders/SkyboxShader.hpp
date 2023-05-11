#pragma once

#include "rendering/Texture.hpp"

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class SkyboxShader {
public:
    SkyboxShader();
    ~SkyboxShader();

    void use_shader(const glm::mat4& mvp, const Texture& cubemap_texture);
    [[nodiscard]] inline GLuint get_program_id() const noexcept {
        return program_id;
    }

private:
    GLuint program_id = 0;
    GLuint mvp_id = 0;
};
