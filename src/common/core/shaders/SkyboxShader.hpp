#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class SkyboxShader {
public:
    ~SkyboxShader();

    static SkyboxShader& get_instance() {
        static SkyboxShader instance;
        return instance;
    }

    void use_shader(const glm::mat4& mvp);
    [[nodiscard]] inline GLuint get_program_id() const noexcept {
        return program_id;
    }

private:
    SkyboxShader();

    GLuint program_id = 0;
    GLuint mvp_id = 0;
};
