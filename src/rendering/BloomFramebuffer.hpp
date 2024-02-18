#pragma once

#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

#include <array>
#include <vector>

namespace llengine {
class BloomFramebuffer {
public:
    BloomFramebuffer(glm::u32vec2 framebuffer_size, std::uint32_t first_size_divider, std::uint32_t image_stages);
    ~BloomFramebuffer();

    BloomFramebuffer& operator=(BloomFramebuffer&& other) noexcept = default;

    void bind() const;
    [[nodiscard]] const Texture& get_image(std::uint32_t cascade, std::size_t stage) const;
    [[nodiscard]] const std::vector<Texture>& get_image_cascade(std::uint32_t cascade) const;
    [[nodiscard]] glm::u32vec2 get_framebuffer_size() const;

private:
    ManagedFramebufferID framebuffer_id;
    std::vector<Texture> images_cascade_1;
    std::vector<Texture> images_cascade_2;
};
}