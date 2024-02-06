#pragma once

#include "datatypes.hpp"
#include "BloomFramebuffer.hpp"
#include "rendering/shaders/GaussianBlurShader.hpp"

#include <glm/vec2.hpp>

namespace llengine {
class BloomRenderer {
public:
    BloomRenderer(glm::u32vec2 framebuffer_size);
    ~BloomRenderer();

    void assign_framebuffer_size(glm::u32vec2 framebuffer_size);
    void render_to_bloom_texture(const Texture& source_texture, float bloom_radius);
    [[nodiscard]] TextureID get_bloom_texture_id() const;

private:
    BloomFramebuffer framebuffer;
    glm::u32vec2 framebuffer_size;
    GaussianBlurShader blur_shader;
    std::uint8_t ping_pong_index = 0;

    void do_horizontal_blur(const Texture& source_texture, float bloom_radius);
    void do_vertical_blur(float bloom_radius);
    void combine();
};
}