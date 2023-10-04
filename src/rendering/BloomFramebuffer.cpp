#include "BloomFramebuffer.hpp"
#include "rendering/RenderingServer.hpp"

#include <GL/glew.h>
#include <fmt/format.h>

#include <array>

namespace llengine {
BloomFramebuffer::BloomFramebuffer(glm::u32vec2 framebuffer_size, std::uint32_t image_stages) {
    assert(image_stages > 0);

    glGenFramebuffers(1, &framebuffer_id.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    glm::u32vec2 cur_image_size = framebuffer_size;
    for (std::uint32_t i = 0; i < image_stages; i++) {
        Image result;

        cur_image_size /= 2;
        result.size = cur_image_size;

        glGenTextures(1, &result.texture_id.get_ref());
        glBindTexture(GL_TEXTURE_2D, result.texture_id);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, cur_image_size.x,
            cur_image_size.y, 0, GL_RGB, GL_FLOAT, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        images.emplace_back(std::move(result));
    }

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        images[0].texture_id, 0
    );

    std::uint32_t attachment = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &attachment);

    GraphicsAPIEnum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error(fmt::format(
            "Incomplete bloom framebuffer. FBO status: {:x}", fbo_status
        ));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, RenderingServer::get_current_default_framebuffer_id());
}

BloomFramebuffer::~BloomFramebuffer() {

}

void BloomFramebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}

[[nodiscard]] const BloomFramebuffer::Image& BloomFramebuffer::get_image(std::size_t stage) const {
    assert(stage < images.size());
    return images[stage];
}

[[nodiscard]] glm::u32vec2 BloomFramebuffer::get_framebuffer_size() const {
    return images[0].size;
}
}