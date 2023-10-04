#include "BloomRenderer.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderingServer.hpp"

#include "GL/glew.h"

namespace llengine {
using DownsampleShaderType = Shader<"orig_tex_resolution">;
using UpsampleShaderType = Shader<"filter_radius">;

constexpr std::uint32_t IMAGE_STAGES = 5;

BloomRenderer::BloomRenderer(glm::u32vec2 window_size) :
framebuffer(window_size, IMAGE_STAGES),
window_size(window_size) {

}

BloomRenderer::~BloomRenderer() {

}

void BloomRenderer::assign_window_size(glm::u32vec2 window_size) {
    if (this->window_size == window_size) {
        return;
    }

    this->window_size = window_size;
    framebuffer = std::move(BloomFramebuffer(window_size, IMAGE_STAGES));
}

void BloomRenderer::render_to_bloom_texture(TextureID source_texture_id, float bloom_radius) {
    framebuffer.bind();

    render_downsamples(source_texture_id);
    render_upsamples(bloom_radius);

    glBindFramebuffer(GL_FRAMEBUFFER, RenderingServer::get_current_default_framebuffer_id());
    glViewport(0, 0, window_size.x, window_size.y);
}

[[nodiscard]] TextureID BloomRenderer::get_bloom_texture_id() const {
    return framebuffer.get_image(0).texture_id;
}

void BloomRenderer::render_downsamples(TextureID source_texture_id) {
    static DownsampleShaderType shader(
        #include "shaders/bloom_upsample_downsample.vert"
        ,
        #include "shaders/bloom_downsample.frag"
    );

    shader.use_shader();
    shader.set_vec2<"orig_tex_resolution">(framebuffer.get_window_size());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, source_texture_id);

    for (std::uint32_t i = 0; i < IMAGE_STAGES; i++) {
        const auto& cur_image = framebuffer.get_image(i);

        glViewport(0, 0, cur_image.size.x, cur_image.size.y);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            cur_image.texture_id, 0
        );

        Mesh::get_quad()->bind_vao(true, false, false);
        glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
        Mesh::get_quad()->unbind_vao(true, false, false);

        shader.set_vec2<"orig_tex_resolution">(cur_image.size);
        glBindTexture(GL_TEXTURE_2D, cur_image.texture_id);
    }
}

void BloomRenderer::render_upsamples(float filter_radius) {
    static UpsampleShaderType shader(
        #include "shaders/bloom_upsample_downsample.vert"
        ,
        #include "shaders/bloom_upsample.frag"
    );

    shader.use_shader();
    shader.set_float<"filter_radius">(filter_radius);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (std::uint32_t i = IMAGE_STAGES - 1; i > 0; i--) {
        const auto& cur_image = framebuffer.get_image(i);
        const auto& next_image = framebuffer.get_image(i - 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cur_image.texture_id);

        glViewport(0, 0, next_image.size.x, next_image.size.y);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, next_image.texture_id, 0
        );

        Mesh::get_quad()->bind_vao(true, false, false);
        glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
        Mesh::get_quad()->unbind_vao(true, false, false);
    }

    glDisable(GL_BLEND);
}
}