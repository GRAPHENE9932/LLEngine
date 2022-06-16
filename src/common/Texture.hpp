#pragma once

#include <string>

#include <GL/glew.h>

class Texture {
public:
    Texture(GLuint texture_id = 0) noexcept;
    Texture(std::string file_path);
    ~Texture();

    operator GLuint() const noexcept {
        return texture_id;
    }

    void set_id(GLuint new_id);
    GLuint get_id();
    bool get_is_compressed() const;

    /// Load the DDS texture (limited).
    /// Limitations:
    ///  - Only DXT1, DXT3, DXT5 compression formats supported
    ///    (uncompressed don't supported too).
    void load_from_dds(std::string dds_path);

private:
    GLuint texture_id;
    bool is_compressed;
};
