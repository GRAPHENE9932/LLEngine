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
framebuffer(framebuffer_size, FIRST_SIZE_DIVIDER, calculate_best_amount_of_image_stages(framebuffer_size)),
framebuffer_size(framebuffer_size), image_stages(calculate_best_amount_of_image_stages(framebuffer_size)) {

}

BloomRenderer::~BloomRenderer() {

}

void BloomRenderer::assign_framebuffer_size(glm::u32vec2 framebuffer_size) {
    if (this->framebuffer_size == framebuffer_size) {
        return;
    }

    this->framebuffer_size = framebuffer_size;
    this->image_stages = calculate_best_amount_of_image_stages(framebuffer_size);
    framebuffer = std::move(BloomFramebuffer(framebuffer_size, FIRST_SIZE_DIVIDER, image_stages));
}

void BloomRenderer::render_to_bloom_texture(const std::vector<Texture>& source_texture_lods, float bloom_radius) {
    framebuffer.bind();

    float blur_radius = compute_blur_radius(bloom_radius, image_stages);
    do_blur(
        std::span(source_texture_lods.begin() + FIRST_SOURCE_MIP_MAP, source_texture_lods.end()),
        framebuffer.get_image_cascade(0),
        blur_radius,
        false,
        false
    );

    do_blur(
        framebuffer.get_image_cascade(0),
        framebuffer.get_image_cascade(1),
        blur_radius,
        true,
        true
    );

    glBindFramebuffer(GL_FRAMEBUFFER, RenderingServer::get_current_default_framebuffer_id());
    glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
}

[[nodiscard]] TextureID BloomRenderer::get_bloom_texture_id() const {
    return framebuffer.get_image(1, 0).get_id();
}

void BloomRenderer::do_blur(
    std::span<const Texture> source_texture_lods,
    std::span<const Texture> target_texture_lods,
    float blur_radius,
    bool is_vertical,
    bool combine_with_previous_target
) {
    assert(source_texture_lods.size() >= image_stages);
    assert(target_texture_lods.size() >= image_stages);

    for (std::int32_t i = image_stages - 1; i >= 0; i--) {
        float real_radius = blur_radius * std::pow(2u, i);
        if (is_vertical) {
            real_radius *= static_cast<float>(framebuffer_size.x) / framebuffer_size.y;
        }

        const Texture* texture_to_add = nullptr;
        if (combine_with_previous_target && i < image_stages - 1) {
            texture_to_add = &target_texture_lods[i + 1];
        }

        blur_shader.use_shader(source_texture_lods[i], texture_to_add, real_radius, is_vertical);

        const auto& cur_image = target_texture_lods[i];

        glViewport(0, 0, cur_image.get_size().x, cur_image.get_size().y);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            cur_image.get_id(), 0
        );

        Mesh::get_quad()->bind_vao(true, false, false);
        glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
        Mesh::get_quad()->unbind_vao(true, false, false);
    }
}

static void combine_textures(
    const Texture& target, const Texture& source_1, const Texture& source_2,
    const Shader<"source_1", "source_2">& shader
) {
    shader.bind_2d_texture<"source_1">(source_1.get_id(), 0);
    shader.bind_2d_texture<"source_2">(source_2.get_id(), 1);

    glViewport(0, 0, target.get_size().x, target.get_size().y);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        target.get_id(), 0
    );

    Mesh::get_quad()->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
    Mesh::get_quad()->unbind_vao(true, false, false);
}
}
