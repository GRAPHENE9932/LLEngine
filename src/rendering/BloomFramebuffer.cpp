#include "BloomFramebuffer.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/RenderingServer.hpp"
#include "rendering/Texture.hpp"

#include <GL/glew.h>
#include <fmt/format.h>

#include <array>

namespace llengine {
static Texture initialize_texture(glm::u32vec2 size) {
    TextureID texture_id;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, size.x,
        size.y, 0, GL_RGB, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return Texture::from_texture_id(texture_id, size, Texture::Type::TEX_2D);
}

static ManagedFramebufferID initialize_framebuffer(const Texture& texture_to_attach) {
    ManagedFramebufferID framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        texture_to_attach.get_id(), 0
    );

    std::uint32_t attachment = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &attachment);

    GraphicsAPIEnum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error(fmt::format(
            "Incomplete bloom framebuffer. FBO status: {:x}", fbo_status
        ));
    }

    return std::move(framebuffer_id);
}

void BloomFramebuffers::initialize(glm::u32vec2 framebuffer_size, std::uint32_t first_size_divider, std::uint32_t image_stages) {
    assert(image_stages > 0);

    hor.resize(image_stages);
    ver.resize(image_stages);
    glm::u32vec2 cur_size = framebuffer_size / first_size_divider;
    for (std::size_t i = 0; i < image_stages; i++) {
        hor[i].color_attachment = initialize_texture(cur_size);
        ver[i].color_attachment = initialize_texture(cur_size);
        hor[i].id = initialize_framebuffer(hor[i].color_attachment);
        ver[i].id = initialize_framebuffer(ver[i].color_attachment);
        
        cur_size /= 2;
        cur_size = glm::max(cur_size, {1, 1});
    }
}
}