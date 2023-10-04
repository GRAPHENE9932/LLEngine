#pragma once

#include "datatypes.hpp"
#include "BloomFramebuffer.hpp"

#include <glm/vec2.hpp>

namespace llengine {
class BloomRenderer {
public:
    BloomRenderer(glm::u32vec2 window_size);
    ~BloomRenderer();

    void assign_window_size(glm::u32vec2 window_size);
    void render_to_bloom_texture(TextureID source_texture_id, float bloom_radius);
    [[nodiscard]] TextureID get_bloom_texture_id() const;

private:
    BloomFramebuffer framebuffer;
    glm::u32vec2 window_size;

    void render_downsamples(TextureID source_texture_id);
    void render_upsamples(float filter_radius);
};
}