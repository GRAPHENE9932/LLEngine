#pragma once

#include <ios> // std::streamsize
#include <string> // std::string

#include <glm/vec2.hpp> // glm::u32vec2
#include <GL/glew.h> // GLenum

class Texture {
public:
    struct Parameters {
        GLenum magnification_filter = GL_LINEAR;
        GLenum minification_filter = GL_LINEAR;
        GLenum wrap_s = GL_CLAMP_TO_EDGE;
        GLenum wrap_t = GL_CLAMP_TO_EDGE;
        std::string file_path;
        std::streamsize offset = 0;
    };

    Texture() = default;
    explicit inline Texture(GLuint texture_id) noexcept :
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

    [[nodiscard]] inline GLuint get_id() const {
        return texture_id;
    }
    [[nodiscard]] glm::u32vec2 get_size() const {
        return tex_size;
    }

protected:
    GLuint texture_id;
    glm::u32vec2 tex_size {0, 0};
};
