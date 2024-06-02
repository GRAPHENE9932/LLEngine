#include "BloomRenderer.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderingServer.hpp"
#include "rendering/Texture.hpp"

#include <GL/glew.h>

#include <cmath>

namespace llengine {
using DownsampleShaderType = Shader<"orig_tex_resolution">;
using UpsampleShaderType = Shader<"filter_radius">;

template<std::integral T>
T constexpr log2(T n) {
    T result = 0;
    while (n >= 2) {
        result++;
        n /= 2;
    }

    return result;
}

constexpr std::uint32_t FIRST_SIZE_DIVIDER = 4;
constexpr std::uint32_t FIRST_SOURCE_MIP_MAP = log2(FIRST_SIZE_DIVIDER);
constexpr std::uint32_t MIN_LAST_IMAGE_SIZE_X = 64;

static std::uint32_t calculate_best_amount_of_image_stages(glm::u32vec2 framebuffer_size) {
    framebuffer_size.x /= FIRST_SIZE_DIVIDER;
    if (framebuffer_size.x / 2 <= MIN_LAST_IMAGE_SIZE_X) {
        return 1;
    }

    return std::log2(framebuffer_size.x / MIN_LAST_IMAGE_SIZE_X) + 1;
}

static float compute_blur_radius(float bloom_radius, std::uint32_t image_stages) {
    return bloom_radius * std::pow(2.0f, 5.0f - image_stages);
}

BloomRenderer::BloomRenderer(glm::u32vec2 framebuffer_size) :
    framebuffer_size(framebuffer_size), image_stages(calculate_best_amount_of_image_stages(framebuffer_size)) {
    framebuffers.initialize(framebuffer_size, FIRST_SIZE_DIVIDER, image_stages);
}

BloomRenderer::~BloomRenderer() {

}

void BloomRenderer::assign_framebuffer_size(glm::u32vec2 framebuffer_size) {
    if (this->framebuffer_size == framebuffer_size) {
        return;
    }

    this->framebuffer_size = framebuffer_size;
    this->image_stages = calculate_best_amount_of_image_stages(framebuffer_size);
    framebuffers.initialize(framebuffer_size, FIRST_SIZE_DIVIDER, image_stages);
}

void BloomRenderer::render_to_bloom_texture(std::span<const Texture> source_texture_lods, float bloom_radius) {
    float blur_radius = compute_blur_radius(bloom_radius, image_stages);

    do_horizontal_blur(source_texture_lods, bloom_radius);
    do_vertical_blur(bloom_radius);

    glBindFramebuffer(GL_FRAMEBUFFER, rs()._get_main_framebuffer_id());
    glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
}

[[nodiscard]] TextureID BloomRenderer::get_bloom_texture_id() const {
    return framebuffers.ver.at(0).color_attachment;
}

float BloomRenderer::calculate_blur_radius_for_stage(float bloom_radius, std::uint32_t mipmap_level, bool is_vertical) {
    float radius = bloom_radius * std::pow(2u, mipmap_level);
    if (is_vertical) {
        radius *= static_cast<float>(framebuffer_size.x) / framebuffer_size.y;
    }

    return radius;
}

void BloomRenderer::do_horizontal_blur(std::span<const Texture> source_texture_lods, float bloom_radius) {
    for (std::int32_t i = image_stages - 1; i >= 0; i--) {
        float radius = calculate_blur_radius_for_stage(bloom_radius, i + FIRST_SOURCE_MIP_MAP, false);

        do_blur(
            radius,
            false,
            framebuffers.hor.at(i).id,
            framebuffers.hor.at(i).color_attachment.get_size(),
            source_texture_lods[i + FIRST_SOURCE_MIP_MAP],
            nullptr
        );
    }
}

void BloomRenderer::do_vertical_blur(float bloom_radius) {
    for (std::int32_t i = image_stages - 1; i >= 0; i--) {
        float radius = calculate_blur_radius_for_stage(bloom_radius, i + FIRST_SOURCE_MIP_MAP, true);
        const Texture* texture_to_add = i < image_stages - 1 ? &framebuffers.ver.at(i + 1).color_attachment : nullptr;

        do_blur(
            radius,
            true,
            framebuffers.ver.at(i).id,
            framebuffers.ver.at(i).color_attachment.get_size(),
            framebuffers.hor.at(i).color_attachment,
            texture_to_add
        );
    }
}

void BloomRenderer::do_blur(
    float radius,
    bool is_vertical,
    FramebufferID target_fb,
    glm::u32vec2 target_size,
    const Texture& source_tex,
    const Texture* texture_to_add
) {
    blur_shader.use_shader(source_tex, texture_to_add, radius, is_vertical);

    glViewport(0, 0, target_size.x, target_size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, target_fb);

    Mesh::get_quad()->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
    Mesh::get_quad()->unbind_vao(true, false, false);
}
}
