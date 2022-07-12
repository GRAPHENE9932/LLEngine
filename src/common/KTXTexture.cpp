#include <array>
#include <vector>
#include <memory>
#include <numeric>
#include <fstream>
#include <cstring>
#include <stdint.h>
#include <stdexcept>

#include <zstd.h>
#include <glm/vec2.hpp>

#include "KTXTexture.hpp"
#include "VkFormatInfo.hpp"

void ktx_loading_error(std::string_view message, std::string_view file_path) {
    using std::string_literals::operator""s;

    throw std::runtime_error("Failed to load the KTX file. "s + message.data() +
            "\nFile path: \""s + file_path.data() + '\"');
}

template<typename T>
void limit_check(const T value, const T max,
                 std::string_view name, std::string_view file_path) {
    using std::string_literals::operator""s;

    if (value > max) {
        ktx_loading_error(
            "The "s + name.data() + " variable has too large value. "s
            "Have: "s + std::to_string(value) + " Max: "s + std::to_string(max),
            file_path
        );
    }
}

inline void align_stream(std::istream& stream, const std::streamsize boundary) {
    if (boundary <= 1)
        return;

    stream.seekg((stream.tellg() + boundary - static_cast<std::streamsize>(1)) / boundary * boundary);
}

constexpr std::array<uint8_t, 12> KTX_IDENTIFIER {{
    0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
}};

struct Header {
    std::array<uint8_t, 12> identifier;
    uint32_t vk_format;
    uint32_t type_size;
    uint32_t pixel_width;
    uint32_t pixel_height;
    uint32_t pixel_depth;
    uint32_t layer_count;
    uint32_t face_count;
    uint32_t level_count;
    uint32_t supercompression_scheme;
};

struct Index {
    uint32_t dfd_byte_offset;
    uint32_t dfd_byte_length;
    uint32_t kvd_byte_offset;
    uint32_t kvd_byte_length;
    uint64_t sgd_byte_offset;
    uint64_t sgd_byte_length;
};

struct LevelIndex {
    uint64_t byte_offset;
    uint64_t byte_length;
    uint64_t uncompressed_byte_length;
};

enum SupercompressionScheme {
    SC_NONE = 0, SC_BASISLZ = 1, SC_ZSTANDARD = 2,
    SC_ZLIB = 3
};

inline void load_level_data(std::istream& stream, char* const data_ptr,
        const LevelIndex& level_index, const Header& header,
        std::string_view file_path) {
    switch (header.supercompression_scheme) {
    case SC_NONE: {
        stream.read(data_ptr, level_index.byte_length);
        if (!stream)
            ktx_loading_error("Failed to read the level data.", file_path);
        break;
    }
    case SC_ZSTANDARD: {
        std::vector<char> compressed_data(level_index.byte_length);
        stream.read(compressed_data.data(), compressed_data.size());
        if (!stream)
            ktx_loading_error("Failed to read the level data.", file_path);

        std::size_t result {ZSTD_decompress(
            data_ptr, level_index.uncompressed_byte_length,
            compressed_data.data(), compressed_data.size()
        )};
        if (ZSTD_isError(result))
            ktx_loading_error("zstd decompression error.", file_path);
        break;
    }
    default:
        ktx_loading_error("Unsupported supercompression scheme."
                "Only zstd and an no compression at all supported.", file_path);
        break;
    }
}

inline GLuint load_mipmap_levels(std::istream& stream, const Header& header,
        const std::vector<LevelIndex>& level_indexes, std::string_view file_path) {
    const VkFormatInfo format_info {VkFormatInfo::from_vk_format(header.vk_format)};

    const uint32_t mip_padding = header.supercompression_scheme == SC_NONE ?
            std::lcm(format_info.block_size, 4u) : 1u;

    const bool is_cubemap {header.face_count == 6};
    if (header.face_count != 1 && header.face_count != 6)
        ktx_loading_error("Invalid count of faces.", file_path);

    const GLenum tex_target {
        is_cubemap ?
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP) :
        static_cast<GLenum>(GL_TEXTURE_2D)
    };

    // As in the file layout, we will go from the last mipmap level (the smallest image)
    // to the first one (the biggest). But sizes can be computed only in opposite order.
    std::vector<glm::u32vec2> sizes(std::max(header.level_count, 1u));
    sizes[0] = {header.pixel_width, header.pixel_height};
    for (std::size_t i = 1; i < sizes.size(); i++)
        sizes[i] = sizes[i - 1] / 2u;

    GLuint texture_id {};
    glGenTextures(1, &texture_id);
    glBindTexture(tex_target, texture_id);

    const GLenum tex_img_target {
        is_cubemap ?
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X) :
        static_cast<GLenum>(GL_TEXTURE_2D)
    };

    uint32_t mip_level = std::max(header.level_count, 1u);
    do {
        mip_level--;
        align_stream(stream, mip_padding);

        std::vector<char> level_data(level_indexes[mip_level].uncompressed_byte_length);
        load_level_data(stream, level_data.data(), level_indexes[mip_level], header, file_path);

        const uint64_t face_size {format_info.compute_image_size(sizes[mip_level])};

        for (uint32_t face_i = 0; face_i < header.face_count; face_i++) {
            const uint64_t mem_offset {face_size * face_i};

            if (format_info.is_compressed) {
                glCompressedTexImage2D(
                    tex_img_target + face_i, mip_level, format_info.gl_internal_format,
                    sizes[mip_level].x, sizes[mip_level].y, 0,
                    face_size, level_data.data() + mem_offset
                );
            }
            else {
                glTexImage2D(
                    tex_img_target + face_i, mip_level, format_info.gl_internal_format,
                    sizes[mip_level].x, sizes[mip_level].y, 0,
                    format_info.gl_format, format_info.gl_type, level_data.data() + mem_offset
                );
            }
        }
    } while (mip_level > 0);

    // Set parameters.
    glTexParameteri(tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    if (is_cubemap) {
        glTexParameteri(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    return texture_id;
}

KTXTexture::KTXTexture(std::string_view file_path) {
    std::ifstream stream(file_path.data(), std::ios::in | std::ios::binary);
    if (!stream)
        ktx_loading_error("File is not accesible or does not exist.", file_path);

    Header header;
    stream.read(reinterpret_cast<char*>(&header), sizeof(Header));
    if (!stream.good())
        ktx_loading_error("Failed to read the KTX header.", file_path);
    if (header.layer_count > 1)
        ktx_loading_error("Multiple layers are not supported yet.", file_path);

    // Check the identifier.
    if (std::memcmp(header.identifier.data(), KTX_IDENTIFIER.data(), 12) != 0 || !stream.good())
        ktx_loading_error("The KTX identifier is invalid.", file_path);

    // Load indexes.
    Index index;
    stream.read(reinterpret_cast<char*>(&index), sizeof(Index));
    // It's theoretically possible that header.level_count will equal to 4294967295.
    // It will lead to the allocation of 96 GiB. So, limit this value.
    limit_check<uint32_t>(header.level_count, 65536, "levelCount", file_path);
    std::vector<LevelIndex> level_indexes(std::max(1u, header.level_count));
    stream.read(reinterpret_cast<char*>(level_indexes.data()),
            sizeof(LevelIndex) * header.level_count);
    if (!stream.good())
        ktx_loading_error("Failed to read the KTX indexes.", file_path);

    // Load the data format descriptor.
    uint32_t dfd_total_size;
    stream.read(reinterpret_cast<char*>(&dfd_total_size), sizeof(uint32_t));
    limit_check<uint32_t>(dfd_total_size, 16777216, "dfdTotalSize", file_path);
    if (dfd_total_size != 0) {
        dfd_block.resize(dfd_total_size > sizeof(uint32_t) ? dfd_total_size - sizeof(uint32_t) : 0u);
        stream.read(dfd_block.data(), dfd_block.size());
        if (!stream.good())
            ktx_loading_error("Failed to read the data format descriptor.", file_path);
    }

    // Load the key/value data.
    uint32_t kvd_already_read {0};
    uint32_t prev_position {static_cast<uint32_t>(stream.tellg())};
    limit_check<uint32_t>(index.kvd_byte_length, 16777216, "kvdByteLength", file_path);
    while (kvd_already_read < index.kvd_byte_length) {
        // Key and value byte length.
        uint32_t key_and_value_byte_length;
        stream.read(reinterpret_cast<char*>(&key_and_value_byte_length), sizeof(uint32_t));
        limit_check(key_and_value_byte_length, 16777216u, "keyAndValueByteLength", file_path);

        // Key and value data itself.
        key_value_data.push_back({});
        std::getline(stream, key_value_data.back().first, '\0');
        std::getline(stream, key_value_data.back().second, '\0');

        // Align on 4 byte boundary.
        align_stream(stream, 4);

        kvd_already_read += static_cast<uint32_t>(stream.tellg()) - prev_position;
        prev_position = stream.tellg();
    }
    if (!stream.good())
        ktx_loading_error("Failed to read the key/value data.", file_path);

    if (index.sgd_byte_length > 0)
        align_stream(stream, 8);

    // Get supercompression global data.
    limit_check<uint64_t>(index.sgd_byte_length, 16777216, "sgdByteLength", file_path);
    supercompression_global_data.resize(index.sgd_byte_length);
    stream.read(supercompression_global_data.data(), index.sgd_byte_length);
    if (!stream.good())
        ktx_loading_error("Failed to get supercompression global data.", file_path);

    // Set values to the base class.
    this->texture_id = load_mipmap_levels(stream, header, level_indexes, file_path);
    this->tex_size = {header.pixel_width, header.pixel_height};
}
