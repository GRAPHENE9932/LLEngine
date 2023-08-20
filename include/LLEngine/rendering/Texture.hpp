#pragma once

#include <ios> // std::streamsize
#include <string> // std::string
#include <memory>

#include <glm/vec2.hpp> // glm::u32vec2

#include "datatypes.hpp"

namespace llengine {
class NodeProperty;

struct TexLoadingParams {
    GraphicsAPIEnum magnification_filter;
    GraphicsAPIEnum minification_filter;
    GraphicsAPIEnum wrap_s;
    GraphicsAPIEnum wrap_t;
    std::string file_path;
    std::streamsize offset;
    std::streamsize size; // Zero implies that loader must load to the end.

    void set_to_defaults();
};

class TextureLoadingError : std::runtime_error {
public:
    TextureLoadingError(const std::string& message) : std::runtime_error(message) {}
};

class ManagedTextureID {
public:
    ManagedTextureID();
    ManagedTextureID(TextureID id);
    ManagedTextureID(const ManagedTextureID& other) = delete;
    ManagedTextureID(ManagedTextureID&& other) noexcept;
    ~ManagedTextureID();

    ManagedTextureID& operator=(const ManagedTextureID& other) = delete;
    ManagedTextureID& operator=(ManagedTextureID&& other) noexcept;
    operator TextureID() const;

    void set_id(TextureID id);
    [[nodiscard]] TextureID get() const;
    [[nodiscard]] TextureID& get_ref();
    
    void delete_texture();

private:
    TextureID id = 0;
};

class Texture {
public:
    Texture(TextureID texture_id, const glm::u32vec2 tex_size, bool is_cubemap) noexcept :
            texture_id(texture_id), tex_size(tex_size), cubemap(is_cubemap) {}
    Texture(ManagedTextureID&& texture_id, const glm::u32vec2 tex_size, bool is_cubemap) noexcept :
            texture_id(std::move(texture_id)), tex_size(tex_size), cubemap(is_cubemap) {}
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept = default;
    ~Texture() = default;

    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept = default;

    inline operator TextureID() const noexcept {
        return texture_id;
    }

    inline void set_id(TextureID new_id);

    [[nodiscard]] inline TextureID get_id() const {
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
        params.set_to_defaults();
        params.file_path = rgbe_texture_path;
        return Texture::from_rgbe(params);
    }

    [[nodiscard]] static Texture from_ktx(const TexLoadingParams& params);
    [[nodiscard]] static inline Texture from_ktx2(const std::string& ktx_texture_path) {
        TexLoadingParams params;
        params.set_to_defaults();
        params.file_path = ktx_texture_path;
        return Texture::from_ktx(params);
    }

    /**
     * @brief Loads a texture in automatically detected format.
     *
     * If the texture takes the whole file, tries to determine it using the
     * file extension. If extension is invalid or missing, determines by
     * identifiers at the start of texture file.
     */
    [[nodiscard]] static Texture from_file(const TexLoadingParams& params);
    /**
     * @brief Loads a texture in automatically detected format.
     * 
     * Does the same thing as from_file with one parameter, but with hint of the
     * additional parameter: mime_type it will determine the format much faster
     * if this texture is contained inside another file, for example, in glTF.
     *
     * @sa llengine::Texture::from_file(const TexLoadingParams& params)
     */
    [[nodiscard]] static Texture from_file(const TexLoadingParams& params, const std::string& mime_type);
    /**
     * @brief Loads a texture in automatically detected format.
     * 
     * If the file extension is invalid or missing, tries to determine the file
     * type using identifiers at the start of texture file.
     * 
     * @sa llengine::Texture::from_file(const TexLoadingParams& params)
     */
    [[nodiscard]] static inline Texture from_file(const std::string& texture_path) {
        TexLoadingParams params;
        params.set_to_defaults();
        params.file_path = texture_path;
        return Texture::from_file(params);
    }

    [[nodiscard]] static Texture from_property(const NodeProperty& property);

    [[nodiscard]] Texture panorama_to_cubemap() const;
    [[nodiscard]] Texture compute_irradiance_map() const;
    [[nodiscard]] Texture compute_prefiltered_specular_map() const;
    [[nodiscard]] static Texture compute_brdf_integration_map();

protected:
    ManagedTextureID texture_id = 0; // ID of value 0 implies that there are no texture.
    glm::u32vec2 tex_size {0, 0};
    bool cubemap = false;
};
}