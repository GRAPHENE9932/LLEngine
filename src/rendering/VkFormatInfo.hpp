#pragma once

#include <cstdint>

#include <glm/vec2.hpp>

namespace llengine {
struct VkFormatInfo {
    uint32_t gl_format;
    int32_t gl_internal_format;
    uint32_t gl_type;
    uint32_t block_size;
    bool is_compressed;
    glm::u32vec2 block_extents;

    constexpr VkFormatInfo(const uint32_t gl_format, const int32_t gl_internal_format,
            const uint32_t gl_type, const uint32_t block_size,
            const bool is_compressed = false,
            const uint32_t block_width = 1, const uint32_t block_height = 1) :

            gl_format(gl_format), gl_internal_format(gl_internal_format),
            gl_type(gl_type), block_size(block_size),
            is_compressed(is_compressed),
            block_extents(block_width, block_height) {};

    static VkFormatInfo from_vk_format(uint32_t vk_format);

    /// Compute the size (in bytes) of image with specified
    /// width and height in the current format.
    [[nodiscard]] uint64_t compute_image_size(glm::u32vec2 image_extents) const;
};
}