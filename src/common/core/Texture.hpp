#pragma once

#include <ios> // std::streamsize
#include <string> // std::string

#include <glm/vec2.hpp> // glm::u32vec2
#include <GL/glew.h> // GLenum

class Texture {
public:
    inline Texture(GLuint texture_id, const glm::u32vec2 tex_size, bool is_cubemap) noexcept :
            texture_id(texture_id), tex_size(tex_size), cubemap(is_cubemap) {}
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) : Texture(other.texture_id, other.tex_size, other.cubemap) {
        other.texture_id = 0;
        other.tex_size = {0, 0};
        other.cubemap = false;
    }
    inline ~Texture() {
        // This function ignores texture ID of 0.
        glDeleteTextures(1, &texture_id);
    }

    Texture operator=(const Texture& other) = delete;

    inline operator GLuint() const noexcept {
        return texture_id;
    }

    inline void set_id(GLuint new_id) {
        // This function ignores texture ID of 0.
        glDeleteTextures(1, &texture_id);

        texture_id = new_id;
    }

    [[nodiscard]] inline GLuint get_id() const {
        return texture_id;
    }
    [[nodiscard]] glm::u32vec2 get_size() const {
        return tex_size;
    }
    [[nodiscard]] bool is_cubemap() const {
        return cubemap;
    }

protected:
    GLuint texture_id = 0; // ID of value 0 implies that there are no texture.
    glm::u32vec2 tex_size {0, 0};
    bool cubemap = false;
};
