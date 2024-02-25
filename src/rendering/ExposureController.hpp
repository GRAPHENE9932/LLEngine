#pragma once

#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

namespace llengine {
class ExposureController {
public:
    ExposureController(glm::u32vec2 luminance_fb_size);

    void recompute_exposure(const Texture& main_color_texture, float delta_time);
    inline float get_exposure() const {
        return exposure;
    }

private:
    ManagedFramebufferID luminance_framebuffer;
    Texture luminance_attachment;
    float exposure = 1.0f;

    void render_to_luminance_fb(const Texture& main_color_texture) const;
    float extract_average_luminance() const;
};
}