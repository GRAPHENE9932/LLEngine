#pragma once

#include "datatypes.hpp"
#include "BloomFramebuffer.hpp"
#include "rendering/shaders/GaussianBlurShader.hpp"

#include <glm/vec2.hpp>

#include <span>

namespace llengine {
class BloomRenderer {
public:
    BloomRenderer(glm::u32vec2 framebuffer_size);
    ~BloomRenderer();

    void assign_framebuffer_size(glm::u32vec2 framebuffer_size);
    void render_to_bloom_texture(std::span<const Texture> source_texture_lods, float bloom_radius);
    [[nodiscard]] TextureID get_bloom_texture_id() const;

private:
    BloomFramebuffers framebuffers;
    glm::u32vec2 framebuffer_size;
    GaussianBlurShader blur_shader;
    TextureID result_texture_id = 0;

    std::uint32_t image_stages = 4;

    void do_horizontal_blur(std::span<const Texture> source_texture_lods, float bloom_radius);
    void do_vertical_blur(float bloom_radius);
    void do_blur(
        float radius,
        bool is_vertical,
        FramebufferID target_fb,
        glm::u32vec2 target_size,
        const Texture& source_tex,
        const Texture* texture_to_add
    );
    float calculate_blur_radius_for_stage(float bloom_radius, std::uint32_t stage, bool is_vertical);
};
}