#include "MainFramebuffer.hpp"
#include "BloomRenderer.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"

#include <GL/glew.h>

namespace llengine {
MainFramebuffer::MainFramebuffer(glm::u32vec2 size) : bloom_renderer(nullptr), window_size(size) {
    framebuffer.delete_framebuffer();
    glGenFramebuffers(1, &framebuffer.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    initialize_color_attachment(size);
    initialize_depth_attachment(size);

    GraphicsAPIEnum attachment = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &attachment);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Failed to initialize main framebuffer.");
    }
}

MainFramebuffer::~MainFramebuffer() {

}

void MainFramebuffer::render_to_window() {
    if (bloom_enabled) {
        if (!bloom_renderer) {
            bloom_renderer = std::make_unique<BloomRenderer>(window_size);
        }
        bloom_renderer->render_to_bloom_texture(get_color_texture_id(), 0.005f);
    }

    static Shader<"main_image", "bloom_image"> postprocessing_shader(
        #include "shaders/postprocessing.vert"
        ,
        #include "shaders/postprocessing.frag"
    );
    postprocessing_shader.use_shader();
    postprocessing_shader.bind_2d_texture<"main_image">(get_color_texture_id(), 0);
    if (bloom_enabled) {
        postprocessing_shader.bind_2d_texture<"bloom_image">(bloom_renderer->get_bloom_texture_id(), 1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Mesh::get_quad()->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
    Mesh::get_quad()->unbind_vao();
}

[[nodiscard]] FramebufferID MainFramebuffer::get_framebuffer_id() const {
    return framebuffer.get();
}

[[nodiscard]] TextureID MainFramebuffer::get_color_texture_id() const {
    return color_attachment.get();
}

void MainFramebuffer::apply_postprocessing_settings(const QualitySettings& quality_settings) {
    bloom_enabled = quality_settings.enable_bloom;
}

void MainFramebuffer::initialize_color_attachment(glm::u32vec2 size) {
    color_attachment.delete_texture();
    glGenTextures(1, &color_attachment.get_ref());
    glBindTexture(GL_TEXTURE_2D, color_attachment);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, size.x, size.y, 0,
        GL_RGB, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_attachment, 0);
}

void MainFramebuffer::initialize_depth_attachment(glm::u32vec2 size) {
    depth_attachment.delete_renderbuffer();
    glGenRenderbuffers(1, &depth_attachment.get_ref());
    glBindRenderbuffer(GL_RENDERBUFFER, depth_attachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_attachment);
}
}