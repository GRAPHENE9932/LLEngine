#include "BloomFramebuffer.hpp"
#include "rendering/RenderingServer.hpp"
#include "rendering/Texture.hpp"

#include <GL/glew.h>
#include <fmt/format.h>

#include <array>

namespace llengine {
static void initialize_images_cascade(
    std::vector<Texture>& empty_cascade,
    glm::u32vec2 framebuffer_size, std::uint32_t first_size_divider,
    std::uint32_t image_stages
) {
    assert(empty_cascade.empty());

    glm::u32vec2 cur_image_size = framebuffer_size / first_size_divider;
    for (std::uint32_t i = 0; i < image_stages; i++) {
        TextureID texture_id;

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, cur_image_size.x,
            cur_image_size.y, 0, GL_RGB, GL_FLOAT, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        empty_cascade.emplace_back(std::move(Texture::from_texture_id(texture_id, cur_image_size, Texture::Type::TEX_2D)));

        cur_image_size /= 2;
    }
}

BloomFramebuffer::BloomFramebuffer(glm::u32vec2 framebuffer_size, std::uint32_t first_size_divider, std::uint32_t image_stages) {
    assert(image_stages > 0);

    glGenFramebuffers(1, &framebuffer_id.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    initialize_images_cascade(images_cascade_1, framebuffer_size, first_size_divider, image_stages);
    initialize_images_cascade(images_cascade_2, framebuffer_size, first_size_divider, image_stages);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        images_cascade_1[0].get_id(), 0
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

[[nodiscard]] const Texture& BloomFramebuffer::get_image(std::uint32_t cascade, std::size_t stage) const {
    if (cascade == 0) {
        return images_cascade_1.at(stage);
    }
    else if (cascade == 1) {
        return images_cascade_2.at(stage);
    }
    else {
        throw std::runtime_error("Invalid image cascade specified.");
    }
}

[[nodiscard]] const std::vector<Texture>& BloomFramebuffer::get_image_cascade(std::uint32_t cascade) const {
    if (cascade == 0) {
        return images_cascade_1;
    }
    else if (cascade == 1) {
        return images_cascade_2;
    }
    else {
        throw std::runtime_error("Invalid image cascade specified.");
    }
}

[[nodiscard]] glm::u32vec2 BloomFramebuffer::get_framebuffer_size() const {
    return images_cascade_1.at(0).get_size();
}
}