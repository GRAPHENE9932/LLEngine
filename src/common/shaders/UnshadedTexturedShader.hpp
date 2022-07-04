#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class UnshadedTexturedShader {
public:
    ~UnshadedTexturedShader();

    void initialize();
    void use_shader(const glm::mat4& mvp);
    GLuint get_program_id();
    void delete_shader();

    inline bool is_initialized() const noexcept {
        return program_id != 0;
    }

private:
    GLuint program_id = 0;
    GLuint mvp_id;
};

