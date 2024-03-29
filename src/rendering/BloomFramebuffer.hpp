#pragma once

#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

#include <vector>

namespace llengine {
struct BloomFramebuffers {
    struct Framebuffer {
        ManagedFramebufferID id;
        Texture color_attachment;
    };

    std::vector<Framebuffer> hor;
    std::vector<Framebuffer> ver;

    void initialize(glm::u32vec2 framebuffer_size, std::uint32_t first_size_divider, std::uint32_t image_stages);
};
}