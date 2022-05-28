#include <array>
#include <cstring>
#include <fstream>
#include <memory>

#include "utils.hpp"

constexpr uint32_t FOURCC_DXT1 = 0x31545844;
constexpr uint32_t FOURCC_DXT3 = 0x33545844;
constexpr uint32_t FOURCC_DXT5 = 0x35545844;

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

GLuint utils::load_dds(std::string file_path) {
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
        default:
            throw std::runtime_error(
                "Failed to read the texture file: "
                "FourCC have unsupported value (may be unsupported texture compression?)/ "
                "Path to the file:\n" + file_path
            );
            break;
    }

    // Create the texture.
    ManagedTextureID texture_id; // Using managed texture ID in case of error.
    glGenTextures(1, &texture_id.texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Load the mipmaps.
    uint32_t width = header.width;
    uint32_t height = header.height;
    for (GLint mipmap_level = 0;
         mipmap_level < header.mip_map_count && width > 0 && height > 0;
         mipmap_level++) {
        uint32_t size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
        stream.read(buffer.get(), size);
        glCompressedTexImage2D(GL_TEXTURE_2D, mipmap_level, format, width, height,
                               0, size, buffer.get());

        width /= 2;
        height /= 2;
    }

    // Set nearest neighbor interpolation.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return texture_id.release();
}