#include <array>
#include <fstream>
#include <cstring>
#include <memory>

#include "Texture.hpp"

Texture::Texture(GLuint texture_id) noexcept :
    texture_id(texture_id) {

}

Texture::Texture(std::string file_path) {
    load_from_dds(file_path);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture_id);
}

void Texture::set_id(GLuint new_id) {
    if (texture_id != 0)
        glDeleteTextures(1, &texture_id);

    texture_id = new_id;
}

GLuint Texture::get_id() {
    return texture_id;
}

bool Texture::get_is_compressed() const {
    return is_compressed;
}

glm::u32vec2 Texture::get_size() const {
    return tex_size;
}

// BEGIN DDS loading.
constexpr uint32_t FOURCC_DXT1 = 0x31545844;
constexpr uint32_t FOURCC_DXT3 = 0x33545844;
constexpr uint32_t FOURCC_DXT5 = 0x35545844;
constexpr uint32_t FOURCC_NO_COMPR = 0x00000000;

struct DDSPixelFormat {
    uint32_t size;
    uint32_t flags;
    uint32_t four_cc;
    uint32_t rgb_bit_count;
    uint32_t r_bit_mask;
    uint32_t g_bit_mask;
    uint32_t b_bit_mask;
    uint32_t a_bit_mask;
};

struct DDSHeader {
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitch_or_linear_size;
    uint32_t depth;
    uint32_t mip_map_count;
    std::array<uint32_t, 11> reserved_1;
    DDSPixelFormat pixel_format;
    uint32_t caps;
    uint32_t caps_2;
    uint32_t caps_3;
    uint32_t caps_4;
    uint32_t reserved_2;
};

inline GLuint pixel_format_flags_to_format(const uint32_t flags) {
    switch (flags) {
    case 0x00000041:
        return GL_RGBA;
    case 0x00000040:
        return GL_RGB;
    case 0x00000002:
        return GL_ALPHA;
    case 0x00020000:
        return GL_LUMINANCE;
    case 0x00020001:
        return GL_LUMINANCE_ALPHA;
    default:
        return GL_RGBA;
    }
}

void Texture::load_from_dds(std::string file_path) {
    // Open the file.
    std::ifstream stream(file_path, std::ios::binary);
    if (!stream)
        throw std::runtime_error("Failed to open the texture file. Path to the file: " + file_path);

    // Check the magic bytes.
    std::array<char, 4> magic_bytes;
    stream.read(magic_bytes.data(), 4);
    if (std::memcmp(magic_bytes.data(), "DDS ", 4) != 0)
        throw std::runtime_error(
            "Failed to read the texture file: "
            "magic bytes doesn't match. Path to the file:\n" +
            file_path
        );

    // Load the header.
    DDSHeader header;
    stream.read(reinterpret_cast<char*>(&header), sizeof(DDSHeader));

    // Handle the compression format.
    uint32_t block_size;
    GLint format;
    is_compressed = true;
    switch (header.pixel_format.four_cc) {
        case FOURCC_DXT1:
            block_size = 8;
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            block_size = 16;
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            block_size = 16;
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        case FOURCC_NO_COMPR:
            block_size = 1;
            format = pixel_format_flags_to_format(header.pixel_format.flags);
            is_compressed = false;
            break;
        default:
            throw std::runtime_error(
                "Failed to read the texture file: "
                "FourCC have unsupported value (may be unsupported texture compression?)/ "
                "Path to the file:\n" + file_path
            );
            break;
    }

    // Create the texture.
    GLuint new_texture_id;
    glGenTextures(1, &new_texture_id);
    glBindTexture(GL_TEXTURE_2D, new_texture_id);

    // Load the mipmaps.
    tex_size = {header.width, header.height};
    auto cur_tex_size = tex_size;
    for (GLint mipmap_level = 0;
            mipmap_level < header.mip_map_count && cur_tex_size.x > 0 && cur_tex_size.y > 0;
            mipmap_level++) {
        uint32_t size;
        if (is_compressed)
            size = ((cur_tex_size.x + 3) / 4) * ((cur_tex_size.y + 3) / 4) * block_size;
        else
            size = cur_tex_size.x * cur_tex_size.y * header.pixel_format.rgb_bit_count / 8;

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
        stream.read(buffer.get(), size);

        if (is_compressed)
            glCompressedTexImage2D(GL_TEXTURE_2D, mipmap_level, format, cur_tex_size.x, tex_size.y,
                    0, size, buffer.get());
        else
            glTexImage2D(GL_TEXTURE_2D, mipmap_level, GL_RGBA, cur_tex_size.x, cur_tex_size.y,
                    0, format, GL_UNSIGNED_BYTE, buffer.get());

        cur_tex_size /= 2;
    }

    // Set nearest neighbor interpolation.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    set_id(new_texture_id);
}
// END DDS loading.
