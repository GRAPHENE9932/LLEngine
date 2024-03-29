#include "rendering/Texture.hpp"
#include "datatypes.hpp"
#include "rendering/LazyShader.hpp"
#include "rendering/Mesh.hpp"
#include "NodeProperty.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/RenderingServer.hpp"
#include "rendering/Shader.hpp"

#include <glm/mat4x4.hpp>
#include <GL/glew.h>

#include <array>
#include <fstream>
#include <type_traits>
#include <utility>

using namespace llengine;

TexLoadingParams::TexLoadingParams() {
    magnification_filter = GL_LINEAR;
    minification_filter = GL_LINEAR;
    wrap_s = GL_CLAMP_TO_EDGE;
    wrap_t = GL_CLAMP_TO_EDGE;
    file_path = "";
    offset = 0;
    size = 0;
}

ManagedTextureID::ManagedTextureID() = default;

ManagedTextureID::ManagedTextureID(TextureID id) : id(id) {}

ManagedTextureID::ManagedTextureID(ManagedTextureID&& other) noexcept {
    this->id = other.id;
    other.id = 0;
}

ManagedTextureID::~ManagedTextureID() {
    delete_texture();
}

ManagedTextureID& ManagedTextureID::operator=(ManagedTextureID&& other) noexcept {
    set_id(other.id);
    other.id = 0;

    return *this;
}

ManagedTextureID::operator TextureID() const {
    return get();
}

void ManagedTextureID::set_id(TextureID id) {
    delete_texture();
    this->id = id;
}

[[nodiscard]] TextureID ManagedTextureID::get() const {
    return id;
}

[[nodiscard]] TextureID& ManagedTextureID::get_ref() {
    return id;
}

void ManagedTextureID::delete_texture() {
    glDeleteTextures(1, &id);
    id = 0;
}

void Texture::set_id(TextureID new_id) {
    texture_id.set_id(new_id);
}

template<typename T>
static void throw_if_invalid_format(Texture::Format format) {
    switch (format) {
    case Texture::Format::R8:
    case Texture::Format::RG8:
    case Texture::Format::RGB8:
    case Texture::Format::RGBA8:
        if (!std::is_same_v<T, char>) {
            throw std::runtime_error("Invalid input datatype for the specified format.");
        }
        break;
    case Texture::Format::R11G11B10F:
    case Texture::Format::R16F:
    case Texture::Format::RG16F:
    case Texture::Format::RGB16F:
    case Texture::Format::RGBA16F:
    case Texture::Format::R32F:
    case Texture::Format::RG32F:
    case Texture::Format::RGB32F:
    case Texture::Format::RGBA32F:
        if (!std::is_same_v<T, float>) {
            throw std::runtime_error("Invalid input datatype for the specified format.");
        }
        break;
    default:
        throw std::runtime_error("Invalid Texture::Format specified.");
    }
}

static std::pair<GLenum, GLenum> opengl_format_and_internal_format(Texture::Format format) {
    switch (format) {
    case Texture::Format::R8:
        return {GL_RED, GL_R8};
    case Texture::Format::RG8:
        return {GL_RG, GL_RG8};
    case Texture::Format::RGB8:
        return {GL_RGB, GL_RGB8};
    case Texture::Format::RGBA8:
        return {GL_RGBA, GL_RGBA8};
    case Texture::Format::R11G11B10F:
        return {GL_RGB, GL_R11F_G11F_B10F};
    case Texture::Format::R16F:
        return {GL_RED, GL_R16F};
    case Texture::Format::RG16F:
        return {GL_RG, GL_RG16F};
    case Texture::Format::RGB16F:
        return {GL_RGB, GL_RGB16F};
    case Texture::Format::RGBA16F:
        return {GL_RGBA, GL_RGBA16F};
    case Texture::Format::R32F:
        return {GL_RED, GL_R32F};
    case Texture::Format::RG32F:
        return {GL_RG, GL_RG32F};
    case Texture::Format::RGB32F:
        return {GL_RGB, GL_RGB32F};
    case Texture::Format::RGBA32F:
        return {GL_RGBA, GL_RGBA32F};
    default:
        throw std::runtime_error("Invalid Texture::Format specified.");
    }
}

static GLenum opengl_target(Texture::Type type) {
    switch (type) {
    case Texture::Type::TEX_1D:
        return GL_TEXTURE_1D;
    case Texture::Type::TEX_2D:
        return GL_TEXTURE_2D;
    case Texture::Type::TEX_CUBEMAP:
        return GL_TEXTURE_CUBE_MAP;
    default:
        throw std::runtime_error("Invalid Texture::Type specified.");
    }
}

template<typename T>
static GLenum opengl_type() {
    if constexpr (std::is_same_v<T, float>) {
        return GL_FLOAT;
    }
    else if constexpr (std::is_same_v<T, char>) {
        return GL_BYTE;
    }
    else {
        static_assert(false, "Invalid type specified.");
    }
}

[[nodiscard]] Texture Texture::from_texture_id(TextureID texture_id, glm::u32vec2 tex_size, Type type) {
    return Texture(texture_id, tex_size, type);
}

[[nodiscard]] Texture Texture::from_texture_id(ManagedTextureID&& texture_id, glm::u32vec2 tex_size, Type type) {
    return Texture(std::move(texture_id), tex_size, type);
}

template<typename T>
[[nodiscard]] Texture Texture::from_pixel_data(
    T* pixel_data, glm::u32vec2 resolution, Type type, Format format
) {
    GLenum target = opengl_target(type);
    throw_if_invalid_format<T>(format);
    auto [gl_format, gl_internal_format] = opengl_format_and_internal_format(format);

    GLuint texture_id {};
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, texture_id);
    
    switch (type) {
    case Type::TEX_1D:
        glTexImage1D(
            target, 0, gl_internal_format, resolution.x, 0,
            gl_format, std::is_same_v<T, float> ? GL_FLOAT : GL_BYTE, pixel_data
        );
        break;
    case Type::TEX_2D:
        glTexImage2D(
            target, 0, gl_internal_format, resolution.x, resolution.y, 0,
            gl_format, std::is_same_v<T, float> ? GL_FLOAT : GL_BYTE, pixel_data
        );
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    case Type::TEX_CUBEMAP:
        for (std::size_t i = 0; i < 6; i++) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GraphicsAPIEnum>(i), 0, gl_internal_format,
                resolution.x, resolution.y, 0, gl_format, std::is_same_v<T, float> ? GL_FLOAT : GL_BYTE, pixel_data
            );
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        break;
    default:
        throw std::runtime_error("Invalid Texture::Type specified.");
    }
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return Texture(texture_id, resolution, type);
}

template Texture Texture::from_pixel_data<float>(float* pixel_data, glm::u32vec2 resolution, Type type, Format format);
template Texture Texture::from_pixel_data<char>(char* pixel_data, glm::u32vec2 resolution, Type type, Format format);

auto draw_to_cubemap(
    glm::i32vec2 cubemap_size, std::int32_t mipmap_levels, std::invocable<const glm::mat4&, std::int32_t> auto&& drawing_function
) -> Texture {
    const glm::mat4 proj_matrix {glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f)};
    const std::array<glm::mat4, 6> mvp_matrices {
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj_matrix * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    // Allocate the cube map.
    GLuint cubemap_id;
    glActiveTexture(GL_TEXTURE1); // GL_TEXTURE0 will be used for the shader.
    glGenTextures(1, &cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmap_levels - 1);
    glm::i32vec2 cur_cubemap_size = cubemap_size;
    for (std::int32_t level = 0; level < mipmap_levels; level++) {
        for (std::size_t i = 0; i < 6; i++) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GraphicsAPIEnum>(i), level, GL_RGB16F,
                cur_cubemap_size.x, cur_cubemap_size.y, 0, GL_RGB,
                GL_FLOAT, nullptr
            );
        }
        cur_cubemap_size /= 2;
    }

    // Initialize the framebuffer.
    ManagedFramebufferID framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    // Save the state of the viewport to restore it later.
    std::array<GLint, 4> original_viewport_params;
    glGetIntegerv(GL_VIEWPORT, original_viewport_params.data());

    // Draw!
    cur_cubemap_size = cubemap_size;
    for (std::int32_t level = 0; level < mipmap_levels; level++) {
        glViewport(0, 0, cur_cubemap_size.x, cur_cubemap_size.y);
        for (std::size_t i = 0; i < 6; i++) {
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GraphicsAPIEnum>(i),
                cubemap_id, level
            );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            drawing_function(mvp_matrices[i], level);
        }
        cur_cubemap_size /= 2;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, RenderingServer::get_current_default_framebuffer_id());

    // Return viewport to the original state.
    glViewport(
        original_viewport_params[0], original_viewport_params[1],
        original_viewport_params[2], original_viewport_params[3]
    );

    return Texture::from_texture_id(cubemap_id, cubemap_size, Texture::Type::TEX_CUBEMAP);
}

constexpr std::string_view EQUIRECTANGULAR_MAPPER_VERTEX_SHADER_TEXT =
    #include "shaders/equirectangular_mapper.vert"
;
constexpr std::string_view EQUIRECTANGULAR_MAPPER_FRAGMENT_SHADER_TEXT =
    #include "shaders/equirectangular_mapper.frag"
;
static LazyShader<Shader<"mvp">> equirectangular_mapper_shader {EQUIRECTANGULAR_MAPPER_VERTEX_SHADER_TEXT, EQUIRECTANGULAR_MAPPER_FRAGMENT_SHADER_TEXT};

Texture Texture::panorama_to_cubemap() const {
    if (type != Type::TEX_2D) {
        throw std::runtime_error("Unable to make cubemap from panorama because panorama has invalid texture type.");
    }

    const glm::u32vec2 cubemap_size {get_size().x / 2u};
    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    equirectangular_mapper_shader->use_shader();
    return draw_to_cubemap(cubemap_size, 1, [&] (const glm::mat4& mvp, std::int32_t level) {
        equirectangular_mapper_shader->set_mat4<"mvp">(mvp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, get_id());

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao(false, false, false);
    });
}

constexpr std::string_view IRRADIANCE_PRECOMPUTER_VERTEX_SHADER_TEXT =
    #include "shaders/irradiance_precomputer.vert"
;
constexpr std::string_view IRRADIANCE_PRECOMPUTER_FRAGMENT_SHADER_TEXT =
    #include "shaders/irradiance_precomputer.frag"
;
static LazyShader<Shader<"mvp">> irradiance_precomputer_shader {IRRADIANCE_PRECOMPUTER_VERTEX_SHADER_TEXT, IRRADIANCE_PRECOMPUTER_FRAGMENT_SHADER_TEXT};

constexpr glm::u32vec2 IRRADIANCE_MAP_SIZE {16u, 16u};
Texture Texture::compute_irradiance_map() const {
    if (type != Type::TEX_CUBEMAP) {
        throw std::runtime_error("Unable to compute irradiance map because the given environment map is not cubemap.");
    }

    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    irradiance_precomputer_shader->use_shader();
    return draw_to_cubemap(IRRADIANCE_MAP_SIZE, 1, [&] (const glm::mat4& mvp, std::int32_t level) {
        irradiance_precomputer_shader->set_mat4<"mvp">(mvp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, get_id());

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao(false, false, false);
    });
}

constexpr std::string_view SPECULAR_PREFILTER_VERTEX_SHADER_TEXT =
    #include "shaders/specular_prefilter.vert"
;
constexpr std::string_view SPECULAR_PREFILTER_FRAGMENT_SHADER_TEXT =
    #include "shaders/specular_prefilter.frag"
;
static LazyShader<Shader<"mvp", "roughness">> specular_prefilter_shader {SPECULAR_PREFILTER_VERTEX_SHADER_TEXT, SPECULAR_PREFILTER_FRAGMENT_SHADER_TEXT};

constexpr glm::u32vec2 SPECULAR_MAP_SIZE {256u, 256u};
constexpr std::int32_t SPECULAR_MAP_MIPMAP_LEVELS = 9; // log2(256) + 1. Also change LAST_PREFILTERED_MIPMAP_LEVEL in pbr_shader.frag.
Texture Texture::compute_prefiltered_specular_map() const {
    if (type != Type::TEX_CUBEMAP) {
        throw std::runtime_error("Unable to compute specular map because the given environment map is not cubemap.");
    }

    const auto& cube_mesh = Mesh::get_skybox_cube(); // Alias the cube.

    specular_prefilter_shader->use_shader();
    return draw_to_cubemap(SPECULAR_MAP_SIZE, SPECULAR_MAP_MIPMAP_LEVELS, [&] (const glm::mat4& mvp, std::int32_t level) {
        float roughness = static_cast<float>(level) / (SPECULAR_MAP_MIPMAP_LEVELS - 1);
        specular_prefilter_shader->set_mat4<"mvp">(mvp);
        specular_prefilter_shader->set_float<"roughness">(roughness);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, get_id());

        cube_mesh->bind_vao(false, false, false);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, cube_mesh->get_amount_of_vertices(), cube_mesh->get_indices_type(), nullptr);

        cube_mesh->unbind_vao(false, false, false);
    });
}

constexpr std::string_view BRDF_INTEGRATION_MAPPER_VERTEX_SHADER_TEXT =
    #include "shaders/brdf_integration_mapper.vert"
;
constexpr std::string_view BRDF_INTEGRATION_MAPPER_FRAGMENT_SHADER_TEXT =
    #include "shaders/brdf_integration_mapper.frag"
;
static LazyShader<Shader<>> brdf_integration_mapper_shader {BRDF_INTEGRATION_MAPPER_VERTEX_SHADER_TEXT, BRDF_INTEGRATION_MAPPER_FRAGMENT_SHADER_TEXT};

constexpr glm::u32vec2 BRDF_INTEGRATION_MAP_SIZE = SPECULAR_MAP_SIZE;
Texture Texture::compute_brdf_integration_map() {
    const auto& quad_mesh = Mesh::get_quad(); // Alias the cube.

    // Create and allocate the texture.
    GLuint texture_id {};
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RG16F, BRDF_INTEGRATION_MAP_SIZE.x, BRDF_INTEGRATION_MAP_SIZE.y,
        0, GL_RG, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize the framebuffer.
    ManagedFramebufferID framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    // Save the state of the viewport to restore it later.
    std::array<GLint, 4> original_viewport_params;
    glGetIntegerv(GL_VIEWPORT, original_viewport_params.data());
    // Change the viewport.
    glViewport(0, 0, BRDF_INTEGRATION_MAP_SIZE.x, BRDF_INTEGRATION_MAP_SIZE.y);

    // Draw!
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        texture_id, 0
    );

    brdf_integration_mapper_shader->use_shader();

    glClear(GL_COLOR_BUFFER_BIT);
    quad_mesh->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, quad_mesh->get_amount_of_vertices());
    quad_mesh->unbind_vao(true, false, false);

    // Clean up.
    glBindFramebuffer(GL_FRAMEBUFFER, RenderingServer::get_current_default_framebuffer_id());

    // Return viewport to the original state.
    glViewport(
        original_viewport_params[0], original_viewport_params[1],
        original_viewport_params[2], original_viewport_params[3]
    );

    return Texture(texture_id, BRDF_INTEGRATION_MAP_SIZE, Type::TEX_2D);
}

constexpr std::array<std::uint8_t, 12> KTX1_IDENTIFIER {
    0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
};

constexpr std::array<std::uint8_t, 12> KTX2_IDENTIFIER {
    0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
};

constexpr std::array<std::uint8_t, 10> RADIANCE_RGBE_IDENTIFIER {
    0x23, 0x3F, 0x52, 0x41, 0x44, 0x49, 0x41, 0x4E, 0x43, 0x45
};

template<std::size_t ArraySize>
[[nodiscard]] bool stream_starts_with(std::istream& stream, const std::array<std::uint8_t, ArraySize> data) {
    std::array<std::uint8_t, ArraySize> read_data;
    stream.read(reinterpret_cast<char*>(read_data.data()), ArraySize);
    stream.seekg(-static_cast<std::istream::off_type>(ArraySize), std::ios_base::cur);

    if (!stream) {
        stream.clear();
        return false;
    }

    return read_data == data;
}

[[nodiscard]] Texture Texture::from_file(const TexLoadingParams& params) {
    // Try to determine the file type with extension, if the file must be readed as a whole.
    if (params.offset == 0 && params.size == 0) {
        if (params.file_path.ends_with(".ktx") || params.file_path.ends_with(".ktx2")) {
            return from_ktx(params);
        }
        else if (params.file_path.ends_with(".hdr")) {
            return from_rgbe(params);
        }
    }

    // If the texture is just a part of some other file (for instance, glTF),
    // determine the file type using file identifiers at start of the texture.
    std::ifstream stream(params.file_path, std::ios::binary | std::ios::in);
    stream.exceptions(std::ifstream::failbit);
    stream.seekg(params.offset);

    if (stream_starts_with(stream, KTX1_IDENTIFIER) || stream_starts_with(stream, KTX2_IDENTIFIER)) {
        return from_ktx(params);
    }
    else if (stream_starts_with(stream, RADIANCE_RGBE_IDENTIFIER)) {
        return from_rgbe(params);
    }
    else {
        throw std::runtime_error("The format of the specified texture file is unsupported or invalid.");
    }
}

[[nodiscard]] Texture Texture::from_file(const TexLoadingParams& params, const std::string& mime_type) {
    if (mime_type == "image/ktx" || mime_type == "image/ktx2") {
        return from_ktx(params);
    }
    else if (mime_type == "image/x-hdr") {
        return from_rgbe(params);
    }

    return from_file(params);
}

[[nodiscard]] Texture Texture::from_property(const NodeProperty& property) {
    if (property.get<std::string>("type") != "texture") {
        throw std::runtime_error("Failed to load a texture from node property: invalid property type.");
    }

    TexLoadingParams params;
    params.file_path = property.get<std::string>("path");
    params.offset = property.get_optional<std::int64_t>("offset").value_or(0);
    params.size = property.get_optional<std::int64_t>("size").value_or(0);

    return from_file(params);
}