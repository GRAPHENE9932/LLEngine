#pragma once

#include "ManagedRenderbufferID.hpp"
#include "QualitySettings.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

namespace llengine {
class BloomRenderer;

class MainFramebuffer {
public:
    MainFramebuffer(glm::u32vec2 size);
    ~MainFramebuffer();

    void render_to_window();

    [[nodiscard]] FramebufferID get_framebuffer_id() const;
    [[nodiscard]] TextureID get_color_texture_id() const;

    void apply_postprocessing_settings(const QualitySettings& quality_settings);

private:
    glm::u32vec2 window_size;

    ManagedFramebufferID framebuffer;
    ManagedTextureID color_attachment;
    ManagedRenderbufferID depth_attachment;

    bool bloom_enabled = true;
    std::unique_ptr<BloomRenderer> bloom_renderer;

    void initialize_color_attachment(glm::u32vec2 size);
    void initialize_depth_attachment(glm::u32vec2 size);
};
}