#pragma once

#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

#include <vector>

namespace llengine {
class BloomFramebuffer {
struct Image {
    glm::u32vec2 size;
    ManagedTextureID texture_id;
};

public:
    BloomFramebuffer(glm::u32vec2 frambuffer_size, std::uint32_t image_stages);
    ~BloomFramebuffer();

    BloomFramebuffer& operator=(BloomFramebuffer&& other) noexcept = default;

    void bind() const;
    [[nodiscard]] const Image& get_image(std::size_t stage) const;
    [[nodiscard]] glm::u32vec2 get_framebuffer_size() const;

private:
    ManagedFramebufferID framebuffer_id;
    std::vector<Image> images;
};
}