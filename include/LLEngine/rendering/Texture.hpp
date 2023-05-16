#pragma once

#include <ios> // std::streamsize
#include <string> // std::string
#include <memory>

#include <glm/vec2.hpp> // glm::u32vec2
#include <GL/glew.h> // GLenum

class EquirectangularMapperShader;
class IrradiancePrecomputerShader;
class SpecularPrefilterShader;
class BRDFIntegrationMapperShader;

struct TexLoadingParams {
    GLenum magnification_filter = GL_LINEAR;
    GLenum minification_filter = GL_LINEAR;
    GLenum wrap_s = GL_CLAMP_TO_EDGE;
    GLenum wrap_t = GL_CLAMP_TO_EDGE;
    std::string file_path;
    std::streamsize offset = 0;
    std::streamsize size = 0; // Zero implies that loader must load to the end.
};

class TextureLoadingError : std::runtime_error {
public:
    TextureLoadingError(const std::string& message) : std::runtime_error(message) {}
};

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

    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) {
        texture_id = other.texture_id;
        tex_size = other.tex_size;
        cubemap = other.cubemap;
        other.texture_id = 0;
        other.tex_size = {0, 0};
        other.cubemap = false;

        return *this;
    }

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

    [[nodiscard]] static Texture from_rgbe(const TexLoadingParams& params);
    [[nodiscard]] static inline Texture from_rgbe(const std::string& rgbe_texture_path) {
        TexLoadingParams params;
        params.file_path = rgbe_texture_path;
        return Texture::from_rgbe(params);
    }

    [[nodiscard]] static Texture from_ktx2(const TexLoadingParams& params);
    [[nodiscard]] static inline Texture from_ktx2(const std::string& ktx_texture_path) {
        TexLoadingParams params;
        params.file_path = ktx_texture_path;
        return Texture::from_ktx2(params);
    }

    [[nodiscard]] Texture panorama_to_cubemap(EquirectangularMapperShader& shader) const;
    [[nodiscard]] Texture compute_irradiance_map(IrradiancePrecomputerShader& shader) const;
    [[nodiscard]] Texture compute_prefiltered_specular_map(SpecularPrefilterShader& shader) const;
    [[nodiscard]] static Texture compute_brdf_integration_map(BRDFIntegrationMapperShader& shader);

protected:
    GLuint texture_id = 0; // ID of value 0 implies that there are no texture.
    glm::u32vec2 tex_size {0, 0};
    bool cubemap = false;
};
