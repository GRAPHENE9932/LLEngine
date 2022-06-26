#pragma once

#include <glm/vec2.hpp>
#include <GL/glew.h>

class Texture {
public:
    Texture() = default;
    inline Texture(GLuint texture_id) noexcept :
            texture_id(texture_id) {}
    inline ~Texture() {
        glDeleteTextures(1, &texture_id);
    }

    inline operator GLuint() const noexcept {
        return texture_id;
    }

    inline void set_id(GLuint new_id) {
        if (texture_id != 0)
            glDeleteTextures(1, &texture_id);

        texture_id = new_id;
    }

    inline GLuint get_id() const {
        return texture_id;
    }
    glm::u32vec2 get_size() const {
        return tex_size;
    }

protected:
    GLuint texture_id;
    glm::u32vec2 tex_size;
};
